

#include <jni.h>
#include <assert.h>
#include <string.h>
#include <android/log.h>
#include <vector>
#include <android/native_window_jni.h>

#define VK_USE_PLATFORM_ANDROID_KHR
#include <vulkan/vulkan.h>

#include <linux/prctl.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/resource.h>
#include <thread>
#include <string_view>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include <stb_truetype.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma clang diagnostic ignored "-Wsign-compare"

#include "util/logs.hpp"
#include "util/video_provider.h"

#include "Input/product_info.h"
#include "Input/pad_thread.h"

#include "Utilities/cheat_info.h"
#include "Emu/System.h"
#include "Emu/Io/interception.h"
#include "Emu/RSX/RSXThread.h"
#include "Emu/Cell/lv2/sys_usbd.h"

#include "Emu/IdManager.h"

#include "Emu/RSX/RSXDisAsm.h"
#include "Emu/Cell/PPUAnalyser.h"
#include "Emu/Cell/PPUDisAsm.h"
#include "Emu/Cell/PPUThread.h"
#include "Emu/Cell/SPUDisAsm.h"
#include "Emu/Cell/SPUThread.h"
#include "Emu/CPU/CPUThread.h"
#include "Emu/CPU/CPUDisAsm.h"

#include "Emu/Io/Null/NullPadHandler.h"

#include "rpcs3_version.h"
#include "Emu/IdManager.h"
#include "Emu/VFS.h"
#include "Emu/vfs_config.h"
#include "Emu/system_utils.hpp"
#include "Emu/system_config.h"

#include "Emu/system_progress.hpp"
#include "Emu/Cell/Modules/cellGem.h"
#include "Emu/Cell/Modules/cellMsgDialog.h"
#include "Emu/Cell/Modules/cellOskDialog.h"
#include "Emu/Cell/Modules/sceNp.h"

#include "Crypto/unpkg.h"
#include "Crypto/unself.h"
#include "Crypto/unzip.h"
#include "Crypto/decrypt_binaries.h"


#include "Loader/iso.h"
#include "Loader/PUP.h"
#include "Loader/TAR.h"
#include "Loader/PSF.h"
#include "Loader/mself.hpp"

#include "Utilities/File.h"
#include "Utilities/Thread.h"
#include "util/sysinfo.hpp"
#include "util/serialization_ext.hpp"

#include "Emu/Io/PadHandler.h"

#include "Emu/Io/Null/null_camera_handler.h"
#include "Emu/Io/Null/NullKeyboardHandler.h"
#include "Emu/Io/Null/NullMouseHandler.h"
#include "Emu/Io/KeyboardHandler.h"
#include "Emu/Io/MouseHandler.h"

#include "Emu/Audio/Null/NullAudioBackend.h"
#include "Emu/Audio/Cubeb/CubebBackend.h"
#include "Emu/Audio/Null/null_enumerator.h"
#include "Emu/Audio/Cubeb/cubeb_enumerator.h"

#include "Emu/RSX/GSFrameBase.h"

#include "Emu/Io/music_handler_base.h"

#include "Input/raw_mouse_handler.h"

#include "Emu/RSX/VK/VKGSRender.h"

#include "Input/sdl_pad_handler.h"

#include "Emu/Cell/Modules/cellSaveData.h"
#include "Emu/Cell/Modules/sceNpTrophy.h"

#include "Emu/Cell/Modules/cellMusic.h"

#include "Emu/RSX/Overlays/HomeMenu/overlay_home_menu.h"
#include "Emu/RSX/Overlays/overlay_message.h"

#include "yaml-cpp/yaml.h"

#include "Emu/RSX/Overlays/overlay_save_dialog.h"
#include "Emu/Cell/Modules/cellSysutil.h"
#include "Emu/RSX/Overlays/overlay_trophy_notification.h"
#include "Emu/RSX/Overlays/overlay_fonts.h"

#include "cpuinfo.h"
#include "meminfo.h"

#define LOG_TAG "aps3e_native"

#if 1

#define LOGI(...) {}
#define LOGW(...) {}
#define LOGE(...) {}
#define PR {}
#define PRE(f) {}

#else

#define LOGI(...) {      \
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG,"%s : %d",__func__,__LINE__);\
	__android_log_print(ANDROID_LOG_INFO, LOG_TAG,__VA_ARGS__);\
}

#define LOGW(...) {      \
    __android_log_print(ANDROID_LOG_WARN, LOG_TAG,"%s : %d",__func__,__LINE__);\
	__android_log_print(ANDROID_LOG_WARN, LOG_TAG,__VA_ARGS__);\
}

#define LOGE(...) {      \
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,"%s : %d",__func__,__LINE__);\
	__android_log_print(ANDROID_LOG_ERROR, LOG_TAG,__VA_ARGS__);\
}

#define PR {__android_log_print(ANDROID_LOG_WARN, LOG_TAG,"%s : %d",__func__,__LINE__);aps3e_log.notice("%s : %d",__func__,__LINE__);}
#define PRE(f) {__android_log_print(ANDROID_LOG_WARN, LOG_TAG,"EmuCallbacks::%s : %d",f,__LINE__);aps3e_log.notice("EmuCallbacks::%s : %d",f,__LINE__);}

#endif

LOG_CHANNEL(aps3e_log);
LOG_CHANNEL(sys_log, "SYS");

extern void report_fatal_error(std::string_view _text, bool is_html = false, bool include_help_text = true)
{
	aps3e_log.fatal("%s",_text.data());
	LOGE("%s",_text.data());
}

bool is_input_allowed(){
	return true;
}

cfg_input_configurations g_cfg_input_configs;
std::string g_input_config_override;

#include "aps3e_key_handler.cpp"

#include "aps3e_pad_thread.cpp"

#include "pt.cpp"

extern void check_microphone_permissions()
{
}
#ifndef __ANDROID__
void enable_display_sleep()
{
}

void disable_display_sleep()
{
}
#endif
extern void qt_events_aware_op(int repeat_duration_ms, std::function<bool()> wrapped_op)
{
	ensure(wrapped_op);

	{
		while (!wrapped_op())
		{
			if (repeat_duration_ms == 0)
			{
				std::this_thread::yield();
			}
			else if (thread_ctrl::get_current())
			{
				thread_ctrl::wait_for(repeat_duration_ms * 1000);
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(repeat_duration_ms));
			}
		}
	}
}

extern std::shared_ptr<CPUDisAsm> make_disasm(const cpu_thread* cpu, shared_ptr<cpu_thread> handle)
{
	if (!handle)
	{
		switch (cpu->get_class())
		{
		case thread_class::ppu: handle = idm::get_unlocked<named_thread<ppu_thread>>(cpu->id); break;
		case thread_class::spu: handle = idm::get_unlocked<named_thread<spu_thread>>(cpu->id); break;
		default: break;
		}
	}

	std::shared_ptr<CPUDisAsm> result;

	switch (cpu->get_class())
	{
	case thread_class::ppu: result = std::make_shared<PPUDisAsm>(cpu_disasm_mode::interpreter, vm::g_sudo_addr); break;
	case thread_class::spu: result = std::make_shared<SPUDisAsm>(cpu_disasm_mode::interpreter, static_cast<const spu_thread*>(cpu)->ls); break;
	case thread_class::rsx: result = std::make_shared<RSXDisAsm>(cpu_disasm_mode::interpreter, vm::g_sudo_addr, 0, cpu); break;
	default: return result;
	}

	result->set_cpu_handle(std::move(handle));
	return result;
}

template <>
void fmt_class_string<cheat_type>::format(std::string& out, u64 arg)
{
	format_enum(out, arg, [](cheat_type value)
	{
		switch (value)
		{
		case cheat_type::unsigned_8_cheat: return "Unsigned 8 bits";
		case cheat_type::unsigned_16_cheat: return "Unsigned 16 bits";
		case cheat_type::unsigned_32_cheat: return "Unsigned 32 bits";
		case cheat_type::unsigned_64_cheat: return "Unsigned 64 bits";
		case cheat_type::signed_8_cheat: return "Signed 8 bits";
		case cheat_type::signed_16_cheat: return "Signed 16 bits";
		case cheat_type::signed_32_cheat: return "Signed 32 bits";
		case cheat_type::signed_64_cheat: return "Signed 64 bits";
		case cheat_type::max: break;
		}

		return unknown;
	});
}

template <>
void fmt_class_string<std::chrono::sys_time<typename std::chrono::system_clock::duration>>::format(std::string& out, u64 arg)
{
	PR;
	//const std::time_t dateTime = std::chrono::system_clock::to_time_t(get_object(arg));
 	//out += date_time::fmt_time("%Y-%m-%dT%H:%M:%S", dateTime);
}

class android_gs_frame:public GSFrameBase
{
    public:
	ANativeWindow*& wnd;
	int& width;
	int& height;

	android_gs_frame(ANativeWindow*& wnd,int& w,int& h):wnd(wnd),width(w),height(h){PR;
	g_fxo->need<utils::video_provider>();
	}
	~android_gs_frame(){PR;}

	 void close(){PR;}
	 void reset(){PR;}
	 bool shown(){PR;return false;}
	 void hide(){PR;}
	 void show(){PR;}
	 void toggle_fullscreen(){PR;}

	 void delete_context(draw_context_t ctx){PR;}
	 draw_context_t make_context(){PR;return nullptr;}
	 void set_current(draw_context_t ctx){PR;}
	 void flip(draw_context_t ctx, bool skip_frame = false){/*PR;*/}
	 int client_width(){/*PR;*/return this->width;}
	 int client_height(){/*PR;*/return this->height;}
	 f64 client_display_rate(){PR;return 60.0;}
	 bool has_alpha(){PR;return true;}

	 display_handle_t handle() const{PR;return reinterpret_cast<void*>(this->wnd);}

	  bool can_consume_frame() const
{PR;
	utils::video_provider& video_provider = g_fxo->get<utils::video_provider>();
	return video_provider.can_consume_frame();
}

void present_frame(std::vector<u8>& data, u32 pitch, u32 width, u32 height, bool is_bgra) const
{PR;
	utils::video_provider& video_provider = g_fxo->get<utils::video_provider>();
	video_provider.present_frame(data, pitch, width, height, is_bgra);
}

	 void take_screenshot(std::vector<u8>&& sshot_data, u32 sshot_width, u32 sshot_height, bool is_bgra) {PR;}
	 //void take_screenshot(const std::vector<u8> sshot_data, u32 sshot_width, u32 sshot_height, bool is_bgra){PR;}
};

class android_music_handler:public music_handler_base
{
public:
	android_music_handler(){PR;}
	~android_music_handler(){PR;}

	 void stop(){PR;m_state = CELL_MUSIC_PB_STATUS_STOP;}
	 void pause(){PR;m_state = CELL_MUSIC_PB_STATUS_PAUSE;}
	 void play(const std::string& path){PR;m_state = CELL_MUSIC_PB_STATUS_PLAY;}
	 void fast_forward(const std::string& path){PR;m_state = CELL_MUSIC_PB_STATUS_FASTFORWARD;}
	 void fast_reverse(const std::string& path){PR;m_state = CELL_MUSIC_PB_STATUS_FASTREVERSE;}
	 void set_volume(f32 volume){PR;}
	 f32 get_volume() const{PR;return 0;}
};

static const char* localized_string_keys[]={
#define WRAP(x) #x
#include "Emu/localized_string_id.inc"
#undef WRAP
};
static std::string localized_strings[sizeof(localized_string_keys)/sizeof(localized_string_keys[0])];
__attribute__((constructor)) static void init_localized_strings()
{

    constexpr int n=sizeof(localized_string_keys)/sizeof(localized_string_keys[0]);
    for(int i=0;i<n;i++){
        const char* key_id = localized_string_keys[i];
        localized_strings[i]= getenv(key_id)?:"?";
    }
}

class android_save_dialog:public SaveDialogBase{
public:
    s32 ShowSaveDataList(std::vector<SaveDataEntry>& save_entries, s32 focused, u32 op, vm::ptr<CellSaveDataListSet> listSet, bool enable_overlay) override
    {
        LOGI("ShowSaveDataList(save_entries=%d, focused=%d, op=0x%x, listSet=*0x%x, enable_overlay=%d)", save_entries.size(), focused, op, listSet, enable_overlay);

        // TODO: Implement proper error checking in savedata_op?
        const bool use_end = sysutil_send_system_cmd(CELL_SYSUTIL_DRAWING_BEGIN, 0) >= 0;

        if (!use_end)
        {
            LOGE("ShowSaveDataList(): Not able to notify DRAWING_BEGIN callback because one has already been sent!");
        }

        // TODO: Install native shell as an Emu callback
        if (auto manager = g_fxo->try_get<rsx::overlays::display_manager>())
        {
            LOGI("ShowSaveDataList: Showing native UI dialog");

            const s32 result = manager->create<rsx::overlays::save_dialog>()->show(save_entries, focused, op, listSet, enable_overlay);
            if (result != rsx::overlays::user_interface::selection_code::error)
            {
                LOGI("ShowSaveDataList: Native UI dialog returned with selection %d", result);
                if (use_end) sysutil_send_system_cmd(CELL_SYSUTIL_DRAWING_END, 0);
                return result;
            }
            LOGE("ShowSaveDataList: Native UI dialog returned error");
        }

        //if (!Emu.HasGui())
        {
            LOGI("ShowSaveDataList(): Aborting: Emulation has no GUI attached");
            if (use_end) sysutil_send_system_cmd(CELL_SYSUTIL_DRAWING_END, 0);
            return -2;
        }

        // Fall back to front-end GUI
        /*cellSaveData.notice("ShowSaveDataList(): Using fallback GUI");
        atomic_t<s32> selection = 0;

        input::SetIntercepted(true);

        Emu.BlockingCallFromMainThread([&]()
                                       {
                                           save_data_list_dialog sdid(save_entries, focused, op, listSet);
                                           sdid.exec();
                                           selection = sdid.GetSelection();
                                       });

        input::SetIntercepted(false);

        if (use_end) sysutil_send_system_cmd(CELL_SYSUTIL_DRAWING_END, 0);

        return selection.load();*/
    }
    ~android_save_dialog(){}
};

class android_trophy_notification:public TrophyNotificationBase
{
public:
    s32 ShowTrophyNotification(const SceNpTrophyDetails& trophy, const std::vector<uchar>& trophy_icon_buffer) override
    {
        if (auto manager = g_fxo->try_get<rsx::overlays::display_manager>())
        {
            // Allow adding more than one trophy notification. The notification class manages scheduling
            auto popup = std::make_shared<rsx::overlays::trophy_notification>();
            return manager->add(popup, false)->show(trophy, trophy_icon_buffer);
        }

        //if (!Emu.HasGui())
        {
            return 0;
        }
    }
    ~android_trophy_notification(){}
};

class dummy_msg_dialog:public MsgDialogBase
{
public:

    dummy_msg_dialog()=default;
    ~dummy_msg_dialog(){}

    void Create(const std::string& msg, const std::string& title) override
	{
		PRE("dummy_msg_dialog::Create");
        state = MsgDialogState::Open;
        Close(true);
        if(type.button_type.unshifted()==CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO)
            on_close(CELL_MSGDIALOG_BUTTON_YES);
        else if(type.button_type.unshifted()==CELL_MSGDIALOG_TYPE_BUTTON_TYPE_OK)
            on_close(CELL_MSGDIALOG_BUTTON_OK);

	}
	void Close(bool success) override
	{
		PRE("dummy_msg_dialog::Close");
	}
	void SetMsg(const std::string& msg) override
	{
		PRE("dummy_msg_dialog::SetMsg");
	}
	void ProgressBarSetMsg(u32 progressBarIndex, const std::string& msg) override
    {
        PRE("dummy_msg_dialog::ProgressBarSetMsg");
    }
    void ProgressBarReset(u32 progressBarIndex) override
    {
        PRE("dummy_msg_dialog::ProgressBarReset");
    }
    void ProgressBarInc(u32 progressBarIndex, u32 delta) override
    {
        PRE("dummy_msg_dialog::ProgressBarInc");
    }
    void ProgressBarSetValue(u32 progressBarIndex, u32 value) override
    {
        PRE("dummy_msg_dialog::ProgressBarSetValue");
    }
    void ProgressBarSetLimit(u32 index, u32 limit) override
    {
        PRE("dummy_msg_dialog::ProgressBarSetLimit");
    }
};

class dummy_osk_dialog:public OskDialogBase{
public:

    dummy_osk_dialog()=default;
    ~dummy_osk_dialog(){}

    void Create(const OskDialogBase::osk_params& params) override
	{
		PRE("dummy_osk_dialog::Create");
        state = OskDialogState::Open;
        //FIXME
        on_osk_close(CELL_OSKDIALOG_CLOSE_CANCEL);
	}
    void Clear(bool clear_all_data) override
	{
		PRE("dummy_osk_dialog::Clear");
	}
    void Insert(const std::u16string& text) override
	{
		PRE("dummy_osk_dialog::Insert");
	}
    void SetText(const std::u16string& text) override
	{
		PRE("dummy_osk_dialog::SetText");
	}
    void Close(s32 status) override
	{
		PRE("dummy_osk_dialog::Close");
	}
};

class dummy_send_message_dialog:public SendMessageDialogBase
{
public:
    error_code Exec(message_data& msg_data, std::set<std::string>& npids) override
    {
        PRE("dummy_send_message_dialog::Exec");
        return CELL_CANCEL;
    }
    void callback_handler(rpcn::NotificationType ntype, const std::string& username, bool status) override
    {
        PRE("dummy_send_message_dialog::callback_handler");
    }
};

class dummy_recv_message_dialog:public RecvMessageDialogBase
{
public:
    error_code Exec(SceNpBasicMessageMainType type, SceNpBasicMessageRecvOptions options, SceNpBasicMessageRecvAction& recv_result, u64& chosen_msg_id) override
    {
        PRE("dummy_recv_message_dialog::Exec");
        return CELL_CANCEL;
    }
    void callback_handler(const shared_ptr<std::pair<std::string, message_data>> new_msg, u64 msg_id) override
    {
        PRE("dummy_recv_message_dialog::callback_handler");
    }
};

/** RPCS3 emulator has functions it desires to call from the GUI at times. Initialize them in here. */
EmuCallbacks create_emu_cb()
{
	EmuCallbacks callbacks{};

	callbacks.update_emu_settings = [](){PRE("update_emu_settings");};
	callbacks.save_emu_settings = [](){PRE("save_emu_settings");};
	callbacks.init_kb_handler = [](){PRE("init_kb_handler");
		g_fxo->init<KeyboardHandlerBase, NullKeyboardHandler>(Emu.DeserialManager());
	};

	callbacks.init_mouse_handler = []()
	{
		PRE("init_mouse_handler");
#if 0
		mouse_handler handler = g_cfg.io.mouse;

		if (handler == mouse_handler::null)
		{
			switch (g_cfg.io.move)
			{
			case move_handler::mouse:
				LOGW("handler = mouse_handler::basic");
				break;
			case move_handler::raw_mouse:
				LOGW("handler = mouse_handler::raw");
				break;
			default:
				break;
			}
		}

		switch (handler)
		{
		case mouse_handler::null:
		{
			g_fxo->init<MouseHandlerBase, NullMouseHandler>(Emu.DeserialManager());
			break;
		}
		case mouse_handler::basic:
		{
			/*basic_mouse_handler* ret = g_fxo->init<MouseHandlerBase, basic_mouse_handler>(Emu.DeserialManager());
			ensure(ret);
			ret->moveToThread(get_thread());
			ret->SetTargetWindow(m_game_window);
			*/break;
		}
		case mouse_handler::raw:
		{
			g_fxo->init<MouseHandlerBase, raw_mouse_handler>(Emu.DeserialManager());
			break;
		}
		}
#else
        g_fxo->init<MouseHandlerBase, NullMouseHandler>(Emu.DeserialManager());
#endif
	};

	callbacks.init_pad_handler = [](std::string_view title_id)
	{
		PRE("init_pad_handler");
        aps3e_log.warning("init_pad_handler: title_id=%s", title_id);

		ensure(g_fxo->init<named_thread<pad_thread>>(nullptr, nullptr, title_id));
		//extern void process_qt_events();
		//while (!pad::g_started) process_qt_events();
		qt_events_aware_op(0, [](){ return !!pad::g_started; });
	};

	callbacks.get_audio = []() -> std::shared_ptr<AudioBackend>
	{
		PRE("get_audio");
		//std::shared_ptr<AudioBackend> result=std::make_shared<CubebBackend>();
		std::shared_ptr<AudioBackend> result =std::make_shared<NullAudioBackend>();
		switch (g_cfg.audio.renderer.get())
		{
		case audio_renderer::null: result = std::make_shared<NullAudioBackend>(); break;
		case audio_renderer::cubeb: result = std::make_shared<CubebBackend>(); break;
#ifdef HAVE_FAUDIO
		case audio_renderer::faudio: result = std::make_shared<FAudioBackend>(); break;
#endif
		}

		if (!result->Initialized())
		{
			PRE("!result->Initialized()");
			// Fall back to a null backend if something went wrong
			//sys_log.error("Audio renderer %s could not be initialized, using a Null renderer instead. Make sure that no other application is running that might block audio access (e.g. Netflix).", result->GetName());
			result = std::make_shared<NullAudioBackend>();
		}
		return result;
	};

	callbacks.get_audio_enumerator = [](u64 renderer) -> std::shared_ptr<audio_device_enumerator>
	{
		PRE("get_audio_enumerator");
		switch (static_cast<audio_renderer>(renderer))
		{
		case audio_renderer::null: return std::make_shared<null_enumerator>();
#ifdef _WIN32
		case audio_renderer::xaudio: return std::make_shared<xaudio2_enumerator>();
#endif
		case audio_renderer::cubeb: return std::make_shared<cubeb_enumerator>();
#ifdef HAVE_FAUDIO
		case audio_renderer::faudio: return std::make_shared<faudio_enumerator>();
#endif
		default: return std::make_shared<null_enumerator>();// fmt::throw_exception("Invalid renderer index %u", renderer);
		}
	};

	callbacks.get_image_info = [](const std::string& filename, std::string& sub_type, s32& width, s32& height, s32& orientation) -> bool
	{
		PRE("get_image_info");

		/*sub_type.clear();
		width = 0;
		height = 0;
		orientation = 0; // CELL_SEARCH_ORIENTATION_UNKNOWN

		bool success = false;
		Emu.BlockingCallFromMainThread([&]()
		{
			const QImageReader reader(QString::fromStdString(filename));
			if (reader.canRead())
			{
				const QSize size = reader.size();
				width = size.width();
				height = size.height();
				sub_type = reader.subType().toStdString();

				switch (reader.transformation())
				{
				case QImageIOHandler::Transformation::TransformationNone:
					orientation = 1; // CELL_SEARCH_ORIENTATION_TOP_LEFT = 0°
					break;
				case QImageIOHandler::Transformation::TransformationRotate90:
					orientation = 2; // CELL_SEARCH_ORIENTATION_TOP_RIGHT = 90°
					break;
				case QImageIOHandler::Transformation::TransformationRotate180:
					orientation = 3; // CELL_SEARCH_ORIENTATION_BOTTOM_RIGHT = 180°
					break;
				case QImageIOHandler::Transformation::TransformationRotate270:
					orientation = 4; // CELL_SEARCH_ORIENTATION_BOTTOM_LEFT = 270°
					break;
				default:
					// Ignore other transformations for now
					break;
				}

				success = true;
				sys_log.notice("get_image_info found image: filename='%s', sub_type='%s', width=%d, height=%d, orientation=%d", filename, sub_type, width, height, orientation);
			}
			else
			{
				sys_log.error("get_image_info failed to read '%s'. Error='%s'", filename, reader.errorString());
			}
		});*/
		return false;
	};

	callbacks.get_scaled_image = [](const std::string& path, s32 target_width, s32 target_height, s32& width, s32& height, u8* dst, bool force_fit) -> bool
	{
		PRE("get_scaled_image");
		/*width = 0;
		height = 0;

		if (target_width <= 0 || target_height <= 0 || !dst || !fs::is_file(path))
		{
			return false;
		}

		bool success = false;
		Emu.BlockingCallFromMainThread([&]()
		{
			// We use QImageReader instead of QImage. This way we can load and scale image in one step.
			QImageReader reader(QString::fromStdString(path));

			if (reader.canRead())
			{
				QSize size = reader.size();
				width = size.width();
				height = size.height();

				if (width <= 0 || height <= 0)
				{
					return;
				}

				if (force_fit || width > target_width || height > target_height)
				{
					const f32 target_ratio = target_width / static_cast<f32>(target_height);
					const f32 image_ratio = width / static_cast<f32>(height);
					const f32 convert_ratio = image_ratio / target_ratio;

					if (convert_ratio > 1.0f)
					{
						size = QSize(target_width, target_height / convert_ratio);
					}
					else if (convert_ratio < 1.0f)
					{
						size = QSize(target_width * convert_ratio, target_height);
					}
					else
					{
						size = QSize(target_width, target_height);
					}

					reader.setScaledSize(size);
					width = size.width();
					height = size.height();
				}

				QImage image = reader.read();

				if (image.format() != QImage::Format::Format_RGBA8888)
				{
					image = image.convertToFormat(QImage::Format::Format_RGBA8888);
				}

				std::memcpy(dst, image.constBits(), std::min(target_width * target_height * 4LL, image.height() * image.bytesPerLine()));
				success = true;
				sys_log.notice("get_scaled_image scaled image: path='%s', width=%d, height=%d", path, width, height);
			}
			else
			{
				sys_log.error("get_scaled_image failed to read '%s'. Error='%s'", path, reader.errorString());
			}
		});*/
		return false;
	};

	callbacks.resolve_path = [](std::string_view sv)
	{
		LOGW("resolve_path: %s",sv.data());
		if(sv.ends_with("/"sv)){
			sv.remove_suffix(1);
			return std::string{sv};
		}
		return std::string{sv};
		// May result in an empty string if path does not exist
		//return QFileInfo(QString::fromUtf8(sv.data(), static_cast<int>(sv.size()))).canonicalFilePath().toStdString();
	};

	callbacks.on_install_pkgs = [](const std::vector<std::string>& pkgs)
	{
		PRE("on_install_pkgs");
		/*for (const std::string& pkg : pkgs)
		{
			if (!rpcs3::utils::install_pkg(pkg))
			{
				sys_log.error("Failed to install %s", pkg);
				return false;
			}
		}*/
		return true;
	};

	callbacks.try_to_quit = [](bool force_quit, std::function<void()> on_exit) -> bool
	{
		PRE("try_to_quit");
		if (on_exit)
			{
				on_exit();
			}
		/*if (force_quit)
		{
			if (on_exit)
			{
				on_exit();
			}

			quit();
			return true;
		}*/

		return true;
	};
	callbacks.call_from_main_thread = [](std::function<void()> func, atomic_t<u32>* wake_up)
	{
		PRE("call_from_main_thread");

        func();

        if (wake_up)
        {
            *wake_up = true;
            wake_up->notify_one();
        }
	};

	callbacks.init_gs_render = [](utils::serial* ar)
	{
		PRE("init_gs_render");
		switch (g_cfg.video.renderer.get())
		{
		case video_renderer::null:
		{
			LOGE("video_renderer::null");
			break;
		}
		case video_renderer::vulkan:
		{
			g_fxo->init<rsx::thread, named_thread<VKGSRender>>(ar);
			break;
		}
		}
	};

	callbacks.get_camera_handler = []() -> std::shared_ptr<camera_handler_base>
	{
		PRE("get_camera_handler");
		/*switch (g_cfg.io.camera.get())
		{
		case camera_handler::null:
		case camera_handler::fake:
		{
			return std::make_shared<null_camera_handler>();
		}
		case camera_handler::qt:
		{
			fmt::throw_exception("Headless mode can not be used with this camera handler. Current handler: %s", g_cfg.io.camera.get());
		}
		}*/
		return std::make_shared<null_camera_handler>();
	};

	callbacks.get_music_handler = []() -> std::shared_ptr<music_handler_base>
	{
		PRE("get_music_handler");
		/*switch (g_cfg.audio.music.get())
		{
		case music_handler::null:
		{
			return std::make_shared<null_music_handler>();
		}
		case music_handler::qt:
		{
			fmt::throw_exception("Headless mode can not be used with this music handler. Current handler: %s", g_cfg.audio.music.get());
		}
		}*/
		return std::make_shared<android_music_handler>();
	};

	callbacks.get_msg_dialog                 = []() -> std::shared_ptr<MsgDialogBase> {
		PRE("get_msg_dialog");
		return std::make_shared<dummy_msg_dialog>(); };
	callbacks.get_osk_dialog                 = []() -> std::shared_ptr<OskDialogBase> {
		PRE("get_osk_dialog");
		return std::make_shared<dummy_osk_dialog>(); };
	callbacks.get_save_dialog                = []() -> std::unique_ptr<SaveDialogBase> {
		PRE("get_save_dialog");
		return std::make_unique<android_save_dialog>(); };
	callbacks.get_trophy_notification_dialog = []() -> std::unique_ptr<TrophyNotificationBase> {
		PRE("get_trophy_notification_dialog");
		return std::make_unique<android_trophy_notification>(); };
    callbacks.get_sendmessage_dialog=[]()->std::shared_ptr<SendMessageDialogBase>{
        PRE("get_sendmessage_dialog");
        return std::make_shared<dummy_send_message_dialog>();
    };
    callbacks.get_recvmessage_dialog=[]()->std::shared_ptr<RecvMessageDialogBase>{
        PRE("get_recvmessage_dialog");
        return std::make_shared<dummy_recv_message_dialog>();
    };
	callbacks.on_stop   = []() {
		PRE("on_stop");
	};
	callbacks.on_ready  = []() {
		PRE("on_ready");
	};
	callbacks.on_emulation_stop_no_response = [](std::shared_ptr<atomic_t<bool>> closed_successfully, int /*seconds_waiting_already*/)
	{
		PRE("on_emulation_stop_no_response");
		/*if (!closed_successfully || !*closed_successfully)
		{
			report_fatal_error(tr("Stopping emulator took too long."
						"\nSome thread has probably deadlocked. Aborting.").toStdString());
		}*/
	};

	callbacks.on_save_state_progress = [](std::shared_ptr<atomic_t<bool>>, stx::shared_ptr<utils::serial>, stx::atomic_ptr<std::string>*, std::shared_ptr<void>)
	{
		PRE("on_save_state_progress");
	};

	callbacks.enable_disc_eject  = [](bool) {
		PRE("enable_disc_eject");

	};
	callbacks.enable_disc_insert = [](bool) {
		PRE("enable_disc_insert");
	};

	callbacks.on_missing_fw = []() {
		PRE("on_missing_fw");
	};

	callbacks.handle_taskbar_progress = [](s32, s32) {
		PRE("handle_taskbar_progress");

	};

	callbacks.get_localized_string    = [](localized_string_id id, const char* args) -> std::string {
		PRE("get_localized_string");

        std::string str = localized_strings[static_cast<size_t>(id)];
        if(auto it=str.find("%0");it!=std::string::npos) {
            str.replace(it, 2, args);
        }
        return str;
    };
	/*callbacks.get_localized_u32string = [](localized_string_id, const char*) -> std::u32string {
		PRE("get_localized_u32string");
		return {}; };*/

	callbacks.play_sound = [](const std::string&){
		PRE("play_sound");

	};
	callbacks.add_breakpoint = [](u32 /*addr*/){
		PRE("add_breakpoint");

	};

	return callbacks;
}

namespace aps3e_emu{

    pthread_mutex_t key_event_mutex;

    pthread_mutex_t emu_mutex;
    pthread_cond_t emu_cond;

    int emu_status=-1;

    void init();
    bool boot_game();

    struct EmuThr{

        enum{
            STATUS_RUNNING=1,
            STATUS_STOPPED,
            STATUS_PAUSED,
            STATUS_REQUEST_PAUSE,
            STATUS_REQUEST_RESUME,
            STATUS_REQUEST_STOP,
        };


        void operator()(){

            std::string tid=[]{
                std::stringstream ss;
                ss<<std::this_thread::get_id();
                return ss.str();
            }();
            LOGW("new thr: %s",tid.c_str());
            pthread_mutex_init(&key_event_mutex, NULL);

            pthread_mutex_init(&emu_mutex, NULL);
            pthread_cond_init(&emu_cond, NULL);

            init();

            bool boot_ok=boot_game();
#if 1
        emu_status=STATUS_RUNNING;
            while (true){

                if (emu_status == STATUS_REQUEST_RESUME) {
                    pthread_mutex_lock(&emu_mutex);
                    Emu.Resume();
                    emu_status = STATUS_RUNNING;
                    pthread_cond_signal(&emu_cond);

                    pthread_mutex_unlock(&emu_mutex);
                }
                if (emu_status == STATUS_REQUEST_PAUSE) {
                    pthread_mutex_lock(&emu_mutex);
                    Emu.Pause(false,false);
                    emu_status = STATUS_PAUSED;
                    pthread_cond_signal(&emu_cond);
                    pthread_mutex_unlock(&emu_mutex);
                }

                if (emu_status == STATUS_REQUEST_STOP) {
                    pthread_mutex_lock(&emu_mutex);
                    if(boot_ok) Emu.Kill();
                    emu_status = STATUS_STOPPED;
                    pthread_cond_signal(&emu_cond);
                    pthread_mutex_unlock(&emu_mutex);
                    usleep(10);
                    break;
                }
                usleep(10);
            }

            ret:

            pthread_mutex_destroy(&key_event_mutex);
            pthread_mutex_destroy(&emu_mutex);
            pthread_cond_destroy(&emu_cond);
#endif
        }
    } g_emu_thr;

    ANativeWindow* wnd;
    int w;
    int h;

    void init(){

        const char* enable_log=getenv("APS3E_ENABLE_LOG");
        if(enable_log&&strcmp(enable_log,"true")==0){
            static std::unique_ptr<logs::listener> log_file = logs::make_file_listener(std::string(getenv("APS3E_LOG_DIR"))+"/rp3_log.txt", 1024*1024*1024);
        }

        prctl(PR_SET_TIMERSLACK, 50000, 0, 0, 0);

        // Initialize TSC freq (in case it isn't)
        static_cast<void>(utils::get_tsc_freq());

        // Initialize thread pool finalizer (on first use)
        static_cast<void>(named_thread("", [](int) {}));

        struct rlimit64 rlim;
        rlim.rlim_cur = 4096;
        rlim.rlim_max = 4096;

        if (setrlimit64(RLIMIT_NOFILE, &rlim) != 0)
        {
            LOGE("Failed to get max open file limit");
        }
        else{
            LOGW("RLIMIT_NOFILE: rlim_max 0x%x ,rlim_cur 0x%x",rlim.rlim_max,rlim.rlim_cur);
        }

        rlim.rlim_cur = 0x80000000;
        rlim.rlim_max = 0x80000000;
        if (setrlimit64(RLIMIT_MEMLOCK, &rlim) != 0)
        {
            LOGE("Failed to get RLIMIT_MEMLOCK size");
        }
        else{
            LOGW("RLIMIT_MEMLOCK: rlim_max 0x%x ,rlim_cur 0x%x",rlim.rlim_max,rlim.rlim_cur);
        }

        /*if(prlimit64(0,RLIMIT_MEMLOCK,NULL,&rlim)==0){
            LOGW("prlimit64(RLIMIT_MEMLOCK): rlim_max 0x%x ,rlim_cur 0x%x",rlim.rlim_max,rlim.rlim_cur);
        }*/

        EmuCallbacks callbacks=create_emu_cb();

        callbacks.on_run    = [](bool /*start_playtime*/) {
            PRE("on_run");

        };

        callbacks.on_pause  = []() {
            PRE("on_pause");
        };
        callbacks.on_resume = []() {
            PRE("on_resume");
        };

        callbacks.get_gs_frame = [=]() -> std::unique_ptr<GSFrameBase>
        {
            PRE("get_gs_frame");
            return std::unique_ptr<android_gs_frame>(new android_gs_frame(wnd,w,h));
        };

        Emu.SetCallbacks(std::move(callbacks));
        Emu.SetHasGui(true);

        Emu.Init();
    }


    std::string game_id;
    std::string eboot_path;
    int iso_fd;

    bool boot_game(){

        //Emu.CallFromMainThread([=]()
        {
            Emu.SetForceBoot(true);

            //const cfg_mode config_mode = config_path.empty() ? cfg_mode::custom : cfg_mode::config_override;

            const char* config_path=getenv("APS3E_CUSTOM_CONFIG_YAML_PATH");
            const cfg_mode config_mode = config_path?cfg_mode::custom:cfg_mode::global ;

            aps3e_log.warning("iso_fd: %d",iso_fd);
            const game_boot_result error =!eboot_path.empty()? Emu.BootGame(eboot_path, game_id, true, config_mode, config_path?:"")
                    :Emu.BootISO(":PS3_GAME/USRDIR/EBOOT.BIN",game_id,iso_fd,config_mode, config_path?:"");
            LOGW("game_boot_result %d",error);
            return error==game_boot_result::no_errors;
        }//);
    }
}

namespace aps3e_util{

    bool install_firmware(int fd){
        fs::file pup_f=fs::file::from_fd(fd);
        if (!pup_f)
        {
            //LOGE("Error opening PUP file %s (%s)", path);
            LOGE("Firmware installation failed: The selected firmware file couldn't be opened.");
            return false;
        }

        pup_object pup(std::move(pup_f));

        switch (pup.operator pup_error())
        {
            case pup_error::header_read:
            {
                LOGE("%s", pup.get_formatted_error().data());
                LOGE("Firmware installation failed: The provided file is empty.");
                return false;
            }
            case pup_error::header_magic:
            {
                LOGE("Error while installing firmware: provided file is not a PUP file.");
                LOGE("Firmware installation failed: The provided file is not a PUP file.");
                return false;
            }
            case pup_error::expected_size:
            {
                LOGE("%s", pup.get_formatted_error().data());
                LOGE("Firmware installation failed: The provided file is incomplete. Try redownloading it.");
                return false;
            }
            case pup_error::header_file_count:
            case pup_error::file_entries:
            case pup_error::stream:
            {
                std::string error = "Error while installing firmware: PUP file is invalid.";

                if (!pup.get_formatted_error().empty())
                {
                    fmt::append(error, "\n%s", pup.get_formatted_error().data());
                }

                LOGE("%s", error.data());
                LOGE("Firmware installation failed: The provided file is corrupted.");
                return false;
            }
            case pup_error::hash_mismatch:
            {
                LOGE("Error while installing firmware: Hash check failed.");
                LOGE("Firmware installation failed: The provided file's contents are corrupted.");
                return false;
            }
            case pup_error::ok: break;
        }

        fs::file update_files_f = pup.get_file(0x300);

        const usz update_files_size = update_files_f ? update_files_f.size() : 0;

        if (!update_files_size)
        {
            LOGE("Error while installing firmware: Couldn't find installation packages database.");
            LOGE("Firmware installation failed: The provided file's contents are corrupted.");
            return false;
        }

        fs::device_stat dev_stat{};
        if (!fs::statfs(g_cfg_vfs.get_dev_flash(), dev_stat))
        {
            LOGE("Error while installing firmware: Couldn't retrieve available disk space. ('%s')", g_cfg_vfs.get_dev_flash().data());
            LOGE("Firmware installation failed: Couldn't retrieve available disk space.");
            return false;
        }

        if (dev_stat.avail_free < update_files_size)
        {
            LOGE("Error while installing firmware: Out of disk space. ('%s', needed: %d bytes)", g_cfg_vfs.get_dev_flash().data(), update_files_size - dev_stat.avail_free);
            LOGE("Firmware installation failed: Out of disk space.");
            return false;
        }

        tar_object update_files(update_files_f);

        auto update_filenames = update_files.get_filenames();

        update_filenames.erase(std::remove_if(
                                       update_filenames.begin(), update_filenames.end(), [](const std::string& s) { return s.find("dev_flash_") == umax; }),
                               update_filenames.end());

        if (update_filenames.empty())
        {
            LOGE("Error while installing firmware: No dev_flash_* packages were found.");
            LOGE("Firmware installation failed: The provided file's contents are corrupted.");
            return false;
        }

        static constexpr std::string_view cur_version = "4.91";

        std::string version_string;

        if (fs::file version = pup.get_file(0x100))
        {
            version_string = version.to_string();
        }

        if (const usz version_pos = version_string.find('\n'); version_pos != umax)
        {
            version_string.erase(version_pos);
        }

        if (version_string.empty())
        {
            LOGE("Error while installing firmware: No version data was found.");
            LOGE("Firmware installation failed: The provided file's contents are corrupted.");
            return false;
        }

        LOGI("FIRMWARD VER %s",version_string.data());

        if (std::string installed = utils::get_firmware_version(); !installed.empty())
        {
            LOGW("Reinstalling firmware: old=%s, new=%s", installed.data(), version_string.data());
        }

        // Used by tar_object::extract() as destination directory
        g_fxo->reset();
        //g_cfg_vfs.from_default();

        vfs::mount("/dev_flash", g_cfg_vfs.get_dev_flash());

        for (const auto& update_filename : update_filenames)
        {
            auto update_file_stream = update_files.get_file(update_filename);

            if (update_file_stream->m_file_handler)
            {
                // Forcefully read all the data
                update_file_stream->m_file_handler->handle_file_op(*update_file_stream, 0, update_file_stream->get_size(umax), nullptr);
            }

            fs::file update_file = fs::make_stream(std::move(update_file_stream->data));

            SCEDecrypter self_dec(update_file);
            self_dec.LoadHeaders();
            self_dec.LoadMetadata(SCEPKG_ERK, SCEPKG_RIV);
            self_dec.DecryptData();

            auto dev_flash_tar_f = self_dec.MakeFile();
            if (dev_flash_tar_f.size() < 3)
            {
                LOGE("Error while installing firmware: PUP contents are invalid.");
                LOGE("Firmware installation failed: Firmware could not be decompressed");
                //progress = -1;
                return false;
            }

            tar_object dev_flash_tar(dev_flash_tar_f[2]);
            if (!dev_flash_tar.extract())
            {
                LOGE("Error while installing firmware: TAR contents are invalid. (package=%s)", update_filename.data());
                LOGE("The firmware contents could not be extracted."
                     "\nThis is very likely caused by external interference from a faulty anti-virus software."
                     "\nPlease add RPCS3 to your anti-virus\' whitelist or use better anti-virus software.");

                //progress = -1;
                return false;
            }
        }
        update_files_f.close();
        LOGW("install_firmware ok");
        return true;
    }


    /*bool install_pkg(const char* path){
        std::deque<package_reader> readers;
        readers.emplace_back(std::string(path));

        std::deque<std::string> bootable_paths;

        package_install_result result = package_reader::extract_data(readers, bootable_paths);
        LOGE("install_pkg %d %s %s",result.error,result.version.expected.c_str(),result.version.found.c_str());
        return result.error == package_install_result::error_type::no_error;

    }*/
    bool install_pkg(int pkg_fd){
        std::deque<package_reader> readers;
        readers.emplace_back(fs::file::from_fd(pkg_fd));

        std::deque<std::string> bootable_paths;

        package_install_result result = package_reader::extract_data(readers, bootable_paths);
        LOGE("install_pkg %d %s %s",result.error,result.version.expected.c_str(),result.version.found.c_str());
        return result.error == package_install_result::error_type::no_error;

    }

    bool allow_eboot_decrypt(const fs::file& eboot_file){
        fs::file dec_eboot = decrypt_self(eboot_file);
        return dec_eboot?true:false;
    }
}

static void j_setup_game_info(JNIEnv* env,jobject self,jobject meta_info ){

        jclass clsMetaInfo = env->FindClass("aenu/aps3e/Emulator$MetaInfo");
        jfieldID f_eboot = env->GetFieldID(clsMetaInfo, "eboot_path", "Ljava/lang/String;");
        jfieldID f_iso_fd  = env->GetFieldID(clsMetaInfo, "iso_fd", "I");
        jfieldID f_id = env->GetFieldID(clsMetaInfo, "serial", "Ljava/lang/String;");
        jstring f_eboot_v = (jstring) env->GetObjectField(meta_info, f_eboot );
        jstring f_id_v = (jstring) env->GetObjectField(meta_info, f_id);

    const char *id = env->GetStringUTFChars(f_id_v, nullptr);

    aps3e_emu::game_id = std::string(id);

        if(f_eboot_v!=NULL) {
            const char *eboot = env->GetStringUTFChars(f_eboot_v, nullptr);
            aps3e_emu::eboot_path = std::string(eboot);

            env->ReleaseStringUTFChars(f_eboot_v, eboot);
        }else{
            aps3e_emu::iso_fd = env->GetIntField(meta_info, f_iso_fd);
        }

    env->ReleaseStringUTFChars(f_id_v, id);

}

static void j_boot(JNIEnv* env,jobject self){
	LOGW("f_init_pre  ");
	aps3e_log.notice("f_init_pre  ");

	LOGW("ANativeWindow %d %d",aps3e_emu::w,aps3e_emu::h);

	std::thread yy(aps3e_emu::g_emu_thr);
	yy.detach();

	LOGW("f_init_end");
	aps3e_log.notice("f_init_end");

}

static void j_change_surface(JNIEnv* env,jobject self,jint w,jint h){

    aps3e_emu::w=w;
    aps3e_emu::h=h;
}

static void j_setup_surface(JNIEnv* env,jobject self,jobject surface){

    if(aps3e_emu::wnd){
    ANativeWindow_release(aps3e_emu::wnd);
        aps3e_emu::wnd=nullptr;}
    if(surface) {
        aps3e_emu::wnd=ANativeWindow_fromSurface(env,surface);
        aps3e_emu::w=ANativeWindow_getWidth(aps3e_emu::wnd);
        aps3e_emu::h=ANativeWindow_getHeight(aps3e_emu::wnd);
    }
}

#if 0
void boot_disc_thr(const char* path,ANativeWindow* wnd,int32_t w,int32_t h){
	aps3e_init(wnd,w,h);
	Emu.CallFromMainThread([=](){
		Emu.SetForceBoot(true);
		static const std::string g_iso_path=std::string(path);
		const game_boot_result error = Emu.BootDisc(g_iso_path);
		LOGW("game_boot_result %d",error);
	});
}

void boot_disc(JNIEnv* env,jobject self,jobject surface,jstring iso_path){
	LOGW("f_init_pre  ");
	aps3e_log.notice("f_init_pre  ");

	ANativeWindow* wnd=ANativeWindow_fromSurface(env,surface);
	int32_t w=ANativeWindow_getWidth(wnd);
	int32_t h=ANativeWindow_getHeight(wnd);
	LOGW("ANativeWindow %d %d",w,h);

	const char* path = env->GetStringUTFChars(iso_path, nullptr);

	std::thread yy(boot_disc_thr,path,wnd,w,h);
	yy.detach();
	//aps3e_init(wnd,w,h);
	LOGW("f_init_end");
	aps3e_log.notice("f_init_end");
}
#else
static void j_boot_disc(JNIEnv* env,jobject self,jobject surface,jstring iso_path){}

#endif

std::shared_ptr<AndroidVirtualPadHandler> padh=nullptr;
static void j_key_event(JNIEnv* env,jobject self,jint key_code,jboolean pressed,jint value){
	pthread_mutex_lock(&aps3e_emu::key_event_mutex);
	if(!padh){
		auto& pad_thr=g_fxo->get<named_thread<pad_thread>>();
		auto xx=pad_thr.get_handlers().at(pad_handler::keyboard);
		padh=std::dynamic_pointer_cast<AndroidVirtualPadHandler>(xx);
	}
	padh->Key(static_cast<u32>(key_code), static_cast<bool>(pressed),value);
	pthread_mutex_unlock(&aps3e_emu::key_event_mutex);
}

static void j_pause(JNIEnv* env,jobject self){

    pthread_mutex_lock(&aps3e_emu::emu_mutex);
    aps3e_emu::emu_status=aps3e_emu::EmuThr::STATUS_REQUEST_PAUSE;
	while(aps3e_emu::emu_status==aps3e_emu::EmuThr::STATUS_REQUEST_PAUSE)
        pthread_cond_wait(&aps3e_emu::emu_cond,&aps3e_emu::emu_mutex);
    pthread_mutex_unlock(&aps3e_emu::emu_mutex);
}

static void j_resume(JNIEnv* env,jobject self){
    pthread_mutex_lock(&aps3e_emu::emu_mutex);
    aps3e_emu::emu_status=aps3e_emu::EmuThr::STATUS_REQUEST_RESUME;
    while(aps3e_emu::emu_status==aps3e_emu::EmuThr::STATUS_REQUEST_RESUME)
        pthread_cond_wait(&aps3e_emu::emu_cond,&aps3e_emu::emu_mutex);
    pthread_mutex_unlock(&aps3e_emu::emu_mutex);
}

static jboolean j_is_running(JNIEnv* env,jobject self){
    return aps3e_emu::emu_status==aps3e_emu::EmuThr::STATUS_RUNNING;
}

static jboolean j_is_paused(JNIEnv* env,jobject self){
    return aps3e_emu::emu_status==aps3e_emu::EmuThr::STATUS_PAUSED;
}

static void j_quit(JNIEnv* env,jobject self){
    pthread_mutex_lock(&aps3e_emu::emu_mutex);
    aps3e_emu::emu_status=aps3e_emu::EmuThr::STATUS_REQUEST_STOP;
    while(aps3e_emu::emu_status==aps3e_emu::EmuThr::STATUS_REQUEST_STOP)
        pthread_cond_wait(&aps3e_emu::emu_cond,&aps3e_emu::emu_mutex);
    pthread_mutex_unlock(&aps3e_emu::emu_mutex);
}

/*
static jboolean j_install_firmware(JNIEnv* env,jobject self,jstring pup_path){
	jboolean is_copy=false;
	const char* path=env->GetStringUTFChars(pup_path,&is_copy);
	jboolean result= aps3e_util::install_firmware(path);
    env->ReleaseStringUTFChars(pup_path,path);
    return result;
}*/
static jboolean j_install_firmware(JNIEnv* env,jobject self,jint pup_fd){
    //jboolean is_copy=false;
    //const char* path=env->GetStringUTFChars(pup_path,&is_copy);
    jboolean result= aps3e_util::install_firmware(pup_fd);
    //env->ReleaseStringUTFChars(pup_path,path);
    return result;
}

/*
static jboolean j_install_pkg(JNIEnv* env,jobject self,jstring pkg_path){
	jboolean is_copy=false;
	const char* path=env->GetStringUTFChars(pkg_path,&is_copy);
    jboolean result= aps3e_util::install_pkg(path);
    env->ReleaseStringUTFChars(pkg_path,path);
	return result;
}*/

static jboolean j_install_pkg(JNIEnv* env,jobject self,jint pkg_fd){
    //jboolean is_copy=false;
    //const char* path=env->GetStringUTFChars(pkg_path,&is_copy);
    jboolean result= aps3e_util::install_pkg(pkg_fd);
    //env->ReleaseStringUTFChars(pkg_path,path);
    return result;
}

static jobject j_meta_info_from_dir(JNIEnv* env,jobject self,jstring jdir_path){

    auto fetch_psf_path=[](const std::string& dir_path){
        std::string sub_paths[]={
                "/PARAM.SFO","/PS3_GAME/PARAM.SFO"
        };
        for(std::string& sub_path:sub_paths){
            std::string psf_path=dir_path+sub_path;
            if(std::filesystem::exists(psf_path))
                return psf_path;
        }
        return std::string{};
    };

auto fetch_eboot_path=[](const std::string& dir_path){
    std::string sub_paths[]=
            {
                    "/EBOOT.BIN",
                    "/USRDIR/EBOOT.BIN",
                    "/USRDIR/ISO.BIN.EDAT",
                    "/PS3_GAME/USRDIR/EBOOT.BIN",
            };
    for(std::string& sub_path:sub_paths){
        std::string eboot_path=dir_path+sub_path;
        if(std::filesystem::exists(eboot_path))
            return eboot_path;
    }
    return std::string{};
};

    jboolean is_copy=false;
    const char* _dir_path=env->GetStringUTFChars(jdir_path,&is_copy);
    const std::string dir_path=_dir_path;
    env->ReleaseStringUTFChars(jdir_path, _dir_path);

    psf::registry psf=psf::load_object(fetch_psf_path(dir_path));

    jclass cls_MetaInfo=env->FindClass("aenu/aps3e/Emulator$MetaInfo");
    jmethodID mid_MetaInfo_ctor=env->GetMethodID(cls_MetaInfo,"<init>","()V");
    jfieldID fid_MetaInfo_eboot_path=env->GetFieldID(cls_MetaInfo,"eboot_path","Ljava/lang/String;");
    jfieldID fid_MetaInfo_name=env->GetFieldID(cls_MetaInfo,"name","Ljava/lang/String;");
    jfieldID fid_MetaInfo_serial=env->GetFieldID(cls_MetaInfo,"serial","Ljava/lang/String;");
    jfieldID fid_MetaInfo_category=env->GetFieldID(cls_MetaInfo,"category","Ljava/lang/String;");
    jfieldID fid_MetaInfo_version=env->GetFieldID(cls_MetaInfo,"version","Ljava/lang/String;");
    jfieldID fid_MetaInfo_decrypt=env->GetFieldID(cls_MetaInfo,"decrypt","Z");

    jobject meta_info=env->NewObject(cls_MetaInfo,mid_MetaInfo_ctor);

    std::string v;
    env->SetObjectField(meta_info,fid_MetaInfo_eboot_path,env->NewStringUTF((v=fetch_eboot_path(dir_path)).c_str()));
    env->SetBooleanField(meta_info,fid_MetaInfo_decrypt,aps3e_util::allow_eboot_decrypt(fs::file(v)));
    env->SetObjectField(meta_info,fid_MetaInfo_name,env->NewStringUTF((v=psf::get_string(psf,"TITLE","???")).c_str()));
    env->SetObjectField(meta_info,fid_MetaInfo_serial,env->NewStringUTF((v=psf::get_string(psf,"TITLE_ID","???")).c_str()));
    env->SetObjectField(meta_info,fid_MetaInfo_category,env->NewStringUTF((v=psf::get_string(psf,"CATEGORY","???")).c_str()));
    env->SetObjectField(meta_info,fid_MetaInfo_version,env->NewStringUTF((v=psf::get_string(psf,"VERSION","???")).c_str()));
    return meta_info;
}


static jobject j_meta_info_from_iso(JNIEnv* env,jobject self,jint fd,jstring jiso_uri_path){

    jclass cls_MetaInfo=env->FindClass("aenu/aps3e/Emulator$MetaInfo");
    jmethodID mid_MetaInfo_ctor=env->GetMethodID(cls_MetaInfo,"<init>","()V");
    jfieldID fid_MetaInfo_iso_uri=env->GetFieldID(cls_MetaInfo,"iso_uri","Ljava/lang/String;");
    jfieldID fid_MetaInfo_name=env->GetFieldID(cls_MetaInfo,"name","Ljava/lang/String;");
    jfieldID fid_MetaInfo_serial=env->GetFieldID(cls_MetaInfo,"serial","Ljava/lang/String;");
    jfieldID fid_MetaInfo_category=env->GetFieldID(cls_MetaInfo,"category","Ljava/lang/String;");
    jfieldID fid_MetaInfo_version=env->GetFieldID(cls_MetaInfo,"version","Ljava/lang/String;");
    jfieldID fid_MetaInfo_icon=env->GetFieldID(cls_MetaInfo,"icon","[B");
    jfieldID fid_MetaInfo_decrypt=env->GetFieldID(cls_MetaInfo,"decrypt","Z");

    std::unique_ptr<iso_fs> iso=iso_fs::from_fd(fd);
    if(!iso->load())
        return NULL;

    if(!iso->exists(":PS3_GAME/USRDIR/EBOOT.BIN"))
        return NULL;

    std::vector<uint8_t> psf_data=iso->get_data_tiny(":PS3_GAME/PARAM.SFO");
    if(psf_data.empty())
        return NULL;

    psf::registry psf=psf::load_object(fs::file(psf_data.data(),psf_data.size()),"PS3_GAME/PARAM.SFO"sv);

    jobject meta_info=env->NewObject(cls_MetaInfo,mid_MetaInfo_ctor);
    env->SetObjectField(meta_info,fid_MetaInfo_iso_uri,jiso_uri_path);
    std::string v;
    env->SetObjectField(meta_info,fid_MetaInfo_name,env->NewStringUTF((v=psf::get_string(psf,"TITLE","???")).c_str()));
    env->SetObjectField(meta_info,fid_MetaInfo_serial,env->NewStringUTF((v=psf::get_string(psf,"TITLE_ID","???")).c_str()));
    env->SetObjectField(meta_info,fid_MetaInfo_category,env->NewStringUTF((v=psf::get_string(psf,"CATEGORY","???")).c_str()));
    env->SetObjectField(meta_info,fid_MetaInfo_version,env->NewStringUTF((v=psf::get_string(psf,"VERSION","???")).c_str()));

    std::vector<uint8_t> icon_data=iso->get_data_tiny(":PS3_GAME/ICON0.PNG");
    if(!icon_data.empty()) {
        jbyteArray icon_array=env->NewByteArray(icon_data.size());
        env->SetByteArrayRegion(icon_array,0,icon_data.size(),reinterpret_cast<const jbyte*>(icon_data.data()));
        env->SetObjectField(meta_info,fid_MetaInfo_icon,icon_array);
    }

    env->SetBooleanField(meta_info,fid_MetaInfo_decrypt,aps3e_util::allow_eboot_decrypt(fs::file(*iso,":PS3_GAME/USRDIR/EBOOT.BIN")));

    return meta_info;
}

#include "aps3e_config.cpp"

//extern "C" int register_Emulator_MetaInfo(C_JNIEnv *env);
extern "C" jobject MetaInfo_from_iso(JNIEnv *env,jobject,jstring);
extern "C" jobject MetaInfo_from_psf(JNIEnv *env,jobject,jstring);
//extern "C" jboolean install_ps3_iso(JNIEnv *env,jobject,jstring,jstring);

/*
static jboolean support_custom_driver(JNIEnv* env,jobject self){
    return access("/dev/kgsl-3d0",F_OK)==0;
}*/

int register_Emulator(JNIEnv* env){

	static const JNINativeMethod methods[] = {


                    {"simple_device_info", "()Ljava/lang/String;",(void *)j_simple_device_info},
            {"get_support_llvm_cpu_list", "()[Ljava/lang/String;",(void *)j_get_support_llvm_cpu_list},
            {"get_vulkan_physical_dev_list", "()[Ljava/lang/String;",(void *)j_get_vulkan_physical_dev_list},

            //{"support_custom_driver","()Z",(void *)support_custom_driver},

            { "generate_config_xml", "()Ljava/lang/String;", (void *) generate_config_xml },
            {"generate_strings_xml","()Ljava/lang/String;", (void *)generate_strings_xml},
            {"generate_java_string_arr","()Ljava/lang/String;", (void *)generate_java_string_arr},

        { "install_firmware", "(I)Z", (void *) j_install_firmware },
		//{ "meta_info_from_iso","(Ljava/lang/String;)Laenu/aps3e/Emulator$MetaInfo;",(void*)MetaInfo_from_iso},
		{ "meta_info_from_dir","(Ljava/lang/String;)Laenu/aps3e/Emulator$MetaInfo;",(void*)j_meta_info_from_dir},

            { "meta_info_from_iso","(ILjava/lang/String;)Laenu/aps3e/Emulator$MetaInfo;",(void*)j_meta_info_from_iso},

        //{ "setup_context", "(Landroid/content/Context;)V", (void *) j_setup_context },

        { "setup_game_info", "(Laenu/aps3e/Emulator$MetaInfo;)V", (void *) j_setup_game_info },
		{ "boot", "()V", (void *) j_boot },
		//{ "boot_disc", "(Landroid/view/Surface;Ljava/lang/String;)V", (void *) j_boot_disc },
		{ "pause", "()V", (void *) j_pause},
		{ "resume", "()V", (void *) j_resume},

        { "is_running", "()Z", (void *) j_is_running},
        { "is_paused", "()Z", (void *) j_is_paused},

		{ "quit", "()V", (void *) j_quit},
		{ "key_event", "(IZI)V", (void *) j_key_event},
		//{ "inatall_iso", "(Ljava/lang/String;Ljava/lang/String;)Z", (void *) install_ps3_iso },
		{ "install_pkg", "(I)Z", (void *) j_install_pkg },

        { "change_surface", "(II)V", (void *) j_change_surface },
        { "setup_surface", "(Landroid/view/Surface;)V", (void *) j_setup_surface },
    };

    jclass clazz = env->FindClass("aenu/aps3e/Emulator");
    return env->RegisterNatives(clazz,methods, sizeof(methods)/sizeof(methods[0]));

}

int register_Emulator_cfg(JNIEnv* env){

    static const JNINativeMethod methods[] = {

            { "native_open_config_file", "(Ljava/lang/String;)J", (void *) open_config_file },
            { "native_load_config_entry", "(JLjava/lang/String;)Ljava/lang/String;", (void *) load_config_entry },
            { "native_load_config_entry_ty_arr", "(JLjava/lang/String;)[Ljava/lang/String;", (void *) load_config_entry_ty_arr },
            { "native_save_config_entry", "(JLjava/lang/String;Ljava/lang/String;)V", (void *) save_config_entry },
            { "native_save_config_entry_ty_arr", "(JLjava/lang/String;[Ljava/lang/String;)V", (void *) save_config_entry_ty_arr },
            { "native_close_config_file", "(JLjava/lang/String;)V", (void *) close_config_file },
    };
    jclass clazz = env->FindClass("aenu/aps3e/Emulator$Config");
    return env->RegisterNatives(clazz,methods, sizeof(methods)/sizeof(methods[0]));
}
/*
static JavaVM* g_jvm;

extern "C" jint JNI_GetCreatedJavaVMs(JavaVM** pvm, jsize count, jsize* found){
	if(count!=1){
		LOGE("????JNI_GetCreatedJavaVMs");
	}
	*pvm=g_jvm;
	*found=1;
	return JNI_OK;
}*/

/*
int main() {
    std::vector<MemoryMapEntry> maps = parseProcSelfMaps();
    for (const auto& entry : maps) {
        std::cout << "Start: " << entry.start_addr << ", End: " << entry.end_addr
                  << ", Permissions: " << entry.permissions << ", Offset: " << entry.offset
                  << ", Device: " << entry.dev_major << ":" << entry.dev_minor
                  << ", Inode: " << entry.inode << ", Pathname: " << entry.pathname << std::endl;
    }
    return 0;
}*/


//esr ctx
  static const esr_context* find_esr_context(const ucontext_t* ctx)
    {
        u32 offset = 0;
        const auto& mctx = ctx->uc_mcontext;

        while ((offset + 4) < sizeof(mctx.__reserved))
        {
            const auto head = reinterpret_cast<const _aarch64_ctx*>(&mctx.__reserved[offset]);
            if (!head->magic)
            {
                // End of linked list
                return nullptr;
            }

            if (head->magic == ESR_MAGIC)
            {
                return reinterpret_cast<const esr_context*>(head);
            }

            offset += head->size;
        }

        return nullptr;
    }

	uint64_t find_esr(const ucontext_t* ctx){
		auto esr_ctx=find_esr_context(ctx);
		if(esr_ctx)return esr_ctx->esr;
		return -1;
	}

static void signal_handler(int /*sig*/, siginfo_t* info, void* uct) noexcept
{
	ucontext_t* ctx = static_cast<ucontext_t*>(uct);
	auto esr_ctx = find_esr_context(ctx);
    if(esr_ctx){
		LOGE("崩了 esr_ctx->esr %d[0x%x] ec %d[0x%x]",esr_ctx->esr,esr_ctx->esr,(esr_ctx->esr>>26)&0b111111,(esr_ctx->esr>>26)&0b111111)
	};
}

extern std::string rp3_get_config_dir(){
    return std::string(getenv("APS3E_DATA_DIR"))+"/config/";
}

extern std::string rp3_get_cache_dir(){
    return std::string (getenv("APS3E_DATA_DIR"))+"/cache/";
}

extern bool cfg_vertex_buffer_upload_mode_use_buffer_view(){
    static const bool r=[]{
        switch(g_cfg.video.vertex_buffer_upload_mode){
            case vertex_buffer_upload_mode::buffer_view:
                return true;
            case vertex_buffer_upload_mode::buffer:
                return false;
            case vertex_buffer_upload_mode::_auto:
                //TODO 默认驱动
                return []()->bool{
                    VkApplicationInfo appinfo = {};
                    appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                    appinfo.pNext = nullptr;
                    appinfo.pApplicationName = "aps3e-cfg-test";
                    appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
                    appinfo.pEngineName = "nul";
                    appinfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
                    appinfo.apiVersion = VK_API_VERSION_1_0;

                    VkInstanceCreateInfo inst_create_info = {};
                    inst_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                    inst_create_info.pApplicationInfo = &appinfo;

                    VkInstance inst;
                    if (vkCreateInstance(&inst_create_info, nullptr, &inst)!= VK_SUCCESS) {
                        __android_log_print(ANDROID_LOG_FATAL, LOG_TAG,"%s : %d",__func__,__LINE__);
                        aps3e_log.fatal("%s : %d",__func__,__LINE__);
                    }

                    // 获取物理设备数量
                    uint32_t physicalDeviceCount = 0;
                    vkEnumeratePhysicalDevices(inst, &physicalDeviceCount, nullptr);
                    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
                    vkEnumeratePhysicalDevices(inst, &physicalDeviceCount, physicalDevices.data());

                    assert(physicalDeviceCount==1);

                    VkPhysicalDeviceProperties deviceProperties;
                    vkGetPhysicalDeviceProperties(physicalDevices[0], &deviceProperties);

                    const bool use_buffer_view=deviceProperties.limits.maxTexelBufferElements>=64*1024*1024;//>=64M
                    vkDestroyInstance(inst, nullptr);

                    return use_buffer_view;

                }();
        }
    }();
    return r;
}

extern const std::unordered_map<rsx::overlays::language_class,std::string>& cfg_font_files(){

      static const auto r=[]->std::unordered_map<rsx::overlays::language_class,std::string>{
          case_lab:
          switch(g_cfg.misc.font_file_selection) {
              case font_file_selection::from_firmware:
                  return {
                          {rsx::overlays::language_class::default_, "SCE-PS3-VR-R-LATIN.TTF"},
                          {rsx::overlays::language_class::cjk_base, "SCE-PS3-SR-R-JPN.TTF"},
                          {rsx::overlays::language_class::hangul,   "SCE-PS3-YG-R-KOR.TTF"}
                  };

              case font_file_selection::from_os:
                  return {
                          {rsx::overlays::language_class::default_, "/system/fonts/Roboto-Regular.ttf"},
                          {rsx::overlays::language_class::cjk_base, "/system/fonts/NotoSansCJK-Regular.ttc"},
                          {rsx::overlays::language_class::hangul,   "/system/fonts/NotoSansCJK-Regular.ttc"}
                  };
              case font_file_selection::custom:
                  std::string custom_font_file_path = g_cfg.misc.custom_font_file_path.to_string();
                  if (!custom_font_file_path.empty() &&
                      std::filesystem::exists(custom_font_file_path))
                      return {
                              {rsx::overlays::language_class::default_, custom_font_file_path},
                              {rsx::overlays::language_class::cjk_base, custom_font_file_path},
                              {rsx::overlays::language_class::hangul,   custom_font_file_path}
                      };
                  else {
                      //Android 15+
                      g_cfg.misc.font_file_selection.set(
                              atoi(getenv("APS3E_ANDROID_API_VERSION")) >= 35
                              ? font_file_selection::from_firmware : font_file_selection::from_os);
                      goto case_lab;
                  }
          };

          return {};
      }();

      return r;
  }

static char* get_process_name() {
    FILE* file = fopen("/proc/self/status", "r");
    if (file == NULL) {
        //perror("Error opening file");
        return NULL;
    }

    char line[256];
    char* name = NULL;
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "Name:", 5) == 0) {
            name = strdup(line + 6);  // 跳过 "Name: "
            name[strcspn(name, "\n")] = '\0';
            break;
        }
    }

    fclose(file);
    return name;
}

extern "C" __attribute__((visibility("default"),section(".aenu.aps3e")))
jint JNI_OnLoad(JavaVM* vm, void* reserved){
	//g_jvm=vm;
	//SDL_JNI_OnLoad(vm,reserved);

    JNIEnv* env = NULL;
    int result=-1;
	char* pn=NULL;
	//const char* emu_pn=getenv("APS3E_EMU_PROC_NAME");

    LOGW("JNI_OnLoad ");
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        LOGE("GetEnv failed");
        goto bail;
    }

	pn=get_process_name();

	LOGW("JNI_OnLoad |%s|",pn);

	if(!pn){
		goto bail;
	}

	if (register_Emulator(env) != JNI_OK) {
			LOGE("register_Emulator failed");
			goto bail;
		}

    if (register_Emulator_cfg(env) != JNI_OK) {
			LOGE("register_Emulator_cfg failed");
			goto bail;
		}

    result = JNI_VERSION_1_6;

    LOGW("JNI_OnLoad OK");


    bail:
	if(pn)free(pn);
        return result;
}


#pragma clang diagnostic pop

