// SPDX-License-Identifier: WTFPL

package aenu.aps3e;
import java.io.*;
import java.util.Base64;
import java.util.HashMap;

import android.content.Context;
import android.os.Build;
import android.os.ParcelFileDescriptor;
import android.view.*;

import org.json.JSONException;
import org.json.JSONObject;

public class Emulator
{
	public static class BootException extends Exception{
	}
	public static class ConfigFileException extends Exception{
	}
	public static class MetaInfo implements Serializable{
		String eboot_path;
		String iso_uri;
		int iso_fd;
		byte[] icon;
		String name;
		String serial;
		String category;
		String version;
		boolean decrypt;

		static JSONObject to_json(MetaInfo  info) throws JSONException {
			JSONObject jo=new JSONObject();
			if(info.eboot_path!=null)
			jo.put("eboot_path",info.eboot_path);
			if(info.iso_uri!=null)
			jo.put("iso_uri",info.iso_uri);
			if(info.icon!=null)
			jo.put("icon",Base64.getEncoder().encodeToString(info.icon));
			jo.put("name",info.name);
			jo.put("serial",info.serial);
			jo.put("category",info.category);
			jo.put("version",info.version);
			jo.put("decrypt",info.decrypt);
			return jo;
		}

		static MetaInfo from_json(JSONObject jo) throws JSONException{
			MetaInfo meta=new MetaInfo();
			if(jo.has("eboot_path"))
			meta.eboot_path=jo.getString("eboot_path");
			if(jo.has("iso_uri"))
			meta.iso_uri=jo.getString("iso_uri");
			if(jo.has("icon"))
			meta.icon=Base64.getDecoder().decode(jo.getString("icon"));
			meta.name=jo.getString("name");
			meta.serial=jo.getString("serial");
			meta.category=jo.getString("category");
			meta.version=jo.getString("version");
			meta.decrypt=jo.getBoolean("decrypt");
			return meta;
		}
	}

	public static class Config{

		String config_path;
		private long n_handle;

		private Config(String config_path) throws ConfigFileException
		{
			this.config_path=config_path;
			if((n_handle=native_open_config_file(config_path))==0)
				throw new ConfigFileException();
		}
		private native long native_open_config_file(String config_path) ;
		private native String native_load_config_entry(long n_handle,String tag);

		private native String[] native_load_config_entry_ty_arr(long n_handle,String tag);
		private native void native_save_config_entry(long n_handle,String tag,String val);

		private native void native_save_config_entry_ty_arr(long n_handle,String tag,String[] val);
		private native void native_close_config_file(long n_handle,String config_path);
		public static Config open_config_file(String config_path) throws ConfigFileException
		{
			return new Config(config_path);
		}

		public String load_config_entry(String tag)
		{
			return native_load_config_entry(n_handle,tag);
		}


		public String[] load_config_entry_ty_arr(String tag)
		{
			return native_load_config_entry_ty_arr(n_handle,tag);
		}
		public void save_config_entry(String tag,String val)
		{
			native_save_config_entry(n_handle,tag,val);
		}

		public void save_config_entry_ty_arr(String tag,String[] val)
		{
			native_save_config_entry_ty_arr(n_handle,tag,val);
		}
		public void close_config_file()
		{
			native_close_config_file(n_handle,config_path);
		}
	}

	/*public static String nc_get_emu_proc_name(){
		return Application.get_emu_proc_name();
	}*/
	
	public final static Emulator get=new Emulator();

	public void setup_env(Application app){
		aenu.lang.System.setenv("APS3E_DATA_DIR",app.get_app_data_dir().getAbsolutePath());
		aenu.lang.System.setenv("APS3E_LOG_DIR",app.get_app_log_dir().getAbsolutePath());
		aenu.lang.System.setenv("APS3E_NATIVE_LIB_DIR",app.get_native_lib_dir());
		aenu.lang.System.setenv("APS3E_ANDROID_API_VERSION", Integer.toString(Build.VERSION.SDK_INT));
		aenu.lang.System.setenv("APS3E_GLOBAL_CONFIG_YAML_PATH", Application.get_global_config_file().getAbsolutePath());

		final String localized_string_keys[] = {
			"INVALID",

					"RSX_OVERLAYS_TROPHY_BRONZE",
					"RSX_OVERLAYS_TROPHY_SILVER",
					"RSX_OVERLAYS_TROPHY_GOLD",
					"RSX_OVERLAYS_TROPHY_PLATINUM",
					"RSX_OVERLAYS_COMPILING_SHADERS",
					"RSX_OVERLAYS_COMPILING_PPU_MODULES",
					"RSX_OVERLAYS_MSG_DIALOG_YES",
					"RSX_OVERLAYS_MSG_DIALOG_NO",
					"RSX_OVERLAYS_MSG_DIALOG_CANCEL",
					"RSX_OVERLAYS_MSG_DIALOG_OK",
					"RSX_OVERLAYS_SAVE_DIALOG_TITLE",
					"RSX_OVERLAYS_SAVE_DIALOG_DELETE",
					"RSX_OVERLAYS_SAVE_DIALOG_LOAD",
					"RSX_OVERLAYS_SAVE_DIALOG_SAVE",
					"RSX_OVERLAYS_OSK_DIALOG_ACCEPT",
					"RSX_OVERLAYS_OSK_DIALOG_CANCEL",
					"RSX_OVERLAYS_OSK_DIALOG_SPACE",
					"RSX_OVERLAYS_OSK_DIALOG_BACKSPACE",
					"RSX_OVERLAYS_OSK_DIALOG_SHIFT",
					"RSX_OVERLAYS_OSK_DIALOG_ENTER_TEXT",
					"RSX_OVERLAYS_OSK_DIALOG_ENTER_PASSWORD",
					"RSX_OVERLAYS_MEDIA_DIALOG_TITLE",
					"RSX_OVERLAYS_MEDIA_DIALOG_TITLE_PHOTO_IMPORT",
					"RSX_OVERLAYS_MEDIA_DIALOG_EMPTY",
					"RSX_OVERLAYS_LIST_SELECT",
					"RSX_OVERLAYS_LIST_CANCEL",
					"RSX_OVERLAYS_LIST_DENY",
					"RSX_OVERLAYS_PRESSURE_INTENSITY_TOGGLED_OFF",
					"RSX_OVERLAYS_PRESSURE_INTENSITY_TOGGLED_ON",
					"RSX_OVERLAYS_ANALOG_LIMITER_TOGGLED_OFF",
					"RSX_OVERLAYS_ANALOG_LIMITER_TOGGLED_ON",
					"RSX_OVERLAYS_MOUSE_AND_KEYBOARD_EMULATED",
					"RSX_OVERLAYS_MOUSE_AND_KEYBOARD_PAD",

					"CELL_GAME_ERROR_BROKEN_GAMEDATA",
					"CELL_GAME_ERROR_BROKEN_HDDGAME",
					"CELL_GAME_ERROR_BROKEN_EXIT_GAMEDATA",
					"CELL_GAME_ERROR_BROKEN_EXIT_HDDGAME",
					"CELL_GAME_ERROR_NOSPACE",
					"CELL_GAME_ERROR_NOSPACE_EXIT",
					"CELL_GAME_ERROR_DIR_NAME",
					"CELL_GAME_DATA_EXIT_BROKEN",
					"CELL_HDD_GAME_EXIT_BROKEN",

					"CELL_HDD_GAME_CHECK_NOSPACE",
					"CELL_HDD_GAME_CHECK_BROKEN",
					"CELL_HDD_GAME_CHECK_NODATA",
					"CELL_HDD_GAME_CHECK_INVALID",

					"CELL_GAMEDATA_CHECK_NOSPACE",
					"CELL_GAMEDATA_CHECK_BROKEN",
					"CELL_GAMEDATA_CHECK_NODATA",
					"CELL_GAMEDATA_CHECK_INVALID",

					"CELL_MSG_DIALOG_ERROR_DEFAULT",
					"CELL_MSG_DIALOG_ERROR_80010001",
					"CELL_MSG_DIALOG_ERROR_80010002",
					"CELL_MSG_DIALOG_ERROR_80010003",
					"CELL_MSG_DIALOG_ERROR_80010004",
					"CELL_MSG_DIALOG_ERROR_80010005",
					"CELL_MSG_DIALOG_ERROR_80010006",
					"CELL_MSG_DIALOG_ERROR_80010007",
					"CELL_MSG_DIALOG_ERROR_80010008",
					"CELL_MSG_DIALOG_ERROR_80010009",
					"CELL_MSG_DIALOG_ERROR_8001000A",
					"CELL_MSG_DIALOG_ERROR_8001000B",
					"CELL_MSG_DIALOG_ERROR_8001000C",
					"CELL_MSG_DIALOG_ERROR_8001000D",
					"CELL_MSG_DIALOG_ERROR_8001000F",
					"CELL_MSG_DIALOG_ERROR_80010010",
					"CELL_MSG_DIALOG_ERROR_80010011",
					"CELL_MSG_DIALOG_ERROR_80010012",
					"CELL_MSG_DIALOG_ERROR_80010013",
					"CELL_MSG_DIALOG_ERROR_80010014",
					"CELL_MSG_DIALOG_ERROR_80010015",
					"CELL_MSG_DIALOG_ERROR_80010016",
					"CELL_MSG_DIALOG_ERROR_80010017",
					"CELL_MSG_DIALOG_ERROR_80010018",
					"CELL_MSG_DIALOG_ERROR_80010019",
					"CELL_MSG_DIALOG_ERROR_8001001A",
					"CELL_MSG_DIALOG_ERROR_8001001B",
					"CELL_MSG_DIALOG_ERROR_8001001C",
					"CELL_MSG_DIALOG_ERROR_8001001D",
					"CELL_MSG_DIALOG_ERROR_8001001E",
					"CELL_MSG_DIALOG_ERROR_8001001F",
					"CELL_MSG_DIALOG_ERROR_80010020",
					"CELL_MSG_DIALOG_ERROR_80010021",
					"CELL_MSG_DIALOG_ERROR_80010022",
					"CELL_MSG_DIALOG_ERROR_80010023",
					"CELL_MSG_DIALOG_ERROR_80010024",
					"CELL_MSG_DIALOG_ERROR_80010025",
					"CELL_MSG_DIALOG_ERROR_80010026",
					"CELL_MSG_DIALOG_ERROR_80010027",
					"CELL_MSG_DIALOG_ERROR_80010028",
					"CELL_MSG_DIALOG_ERROR_80010029",
					"CELL_MSG_DIALOG_ERROR_8001002A",
					"CELL_MSG_DIALOG_ERROR_8001002B",
					"CELL_MSG_DIALOG_ERROR_8001002C",
					"CELL_MSG_DIALOG_ERROR_8001002D",
					"CELL_MSG_DIALOG_ERROR_8001002E",
					"CELL_MSG_DIALOG_ERROR_8001002F",
					"CELL_MSG_DIALOG_ERROR_80010030",
					"CELL_MSG_DIALOG_ERROR_80010031",
					"CELL_MSG_DIALOG_ERROR_80010032",
					"CELL_MSG_DIALOG_ERROR_80010033",
					"CELL_MSG_DIALOG_ERROR_80010034",
					"CELL_MSG_DIALOG_ERROR_80010035",
					"CELL_MSG_DIALOG_ERROR_80010036",
					"CELL_MSG_DIALOG_ERROR_80010037",
					"CELL_MSG_DIALOG_ERROR_80010038",
					"CELL_MSG_DIALOG_ERROR_80010039",
					"CELL_MSG_DIALOG_ERROR_8001003A",
					"CELL_MSG_DIALOG_ERROR_8001003B",
					"CELL_MSG_DIALOG_ERROR_8001003C",
					"CELL_MSG_DIALOG_ERROR_8001003D",
					"CELL_MSG_DIALOG_ERROR_8001003E",

					"CELL_OSK_DIALOG_TITLE",
					"CELL_OSK_DIALOG_BUSY",

					"CELL_SAVEDATA_CB_BROKEN",
					"CELL_SAVEDATA_CB_FAILURE",
					"CELL_SAVEDATA_CB_NO_DATA",
					"CELL_SAVEDATA_CB_NO_SPACE",
					"CELL_SAVEDATA_NO_DATA",
					"CELL_SAVEDATA_NEW_SAVED_DATA_TITLE",
					"CELL_SAVEDATA_NEW_SAVED_DATA_SUB_TITLE",
					"CELL_SAVEDATA_SAVE_CONFIRMATION",
					"CELL_SAVEDATA_DELETE_CONFIRMATION",
					"CELL_SAVEDATA_DELETE_SUCCESS",
					"CELL_SAVEDATA_DELETE",
					"CELL_SAVEDATA_SAVE",
					"CELL_SAVEDATA_LOAD",
					"CELL_SAVEDATA_OVERWRITE",
					"CELL_SAVEDATA_AUTOSAVE",
					"CELL_SAVEDATA_AUTOLOAD",

					"CELL_CROSS_CONTROLLER_MSG",
					"CELL_CROSS_CONTROLLER_FW_MSG",

					"CELL_NP_RECVMESSAGE_DIALOG_TITLE",
					"CELL_NP_RECVMESSAGE_DIALOG_TITLE_INVITE",
					"CELL_NP_RECVMESSAGE_DIALOG_TITLE_ADD_FRIEND",
					"CELL_NP_RECVMESSAGE_DIALOG_FROM",
					"CELL_NP_RECVMESSAGE_DIALOG_SUBJECT",

					"CELL_NP_SENDMESSAGE_DIALOG_TITLE",
					"CELL_NP_SENDMESSAGE_DIALOG_TITLE_INVITE",
					"CELL_NP_SENDMESSAGE_DIALOG_TITLE_ADD_FRIEND",
					"CELL_NP_SENDMESSAGE_DIALOG_CONFIRMATION",
					"CELL_NP_SENDMESSAGE_DIALOG_CONFIRMATION_INVITE",
					"CELL_NP_SENDMESSAGE_DIALOG_CONFIRMATION_ADD_FRIEND",

					"RECORDING_ABORTED",

					"RPCN_NO_ERROR",
					"RPCN_ERROR_INVALID_INPUT",
					"RPCN_ERROR_WOLFSSL",
					"RPCN_ERROR_RESOLVE",
					"RPCN_ERROR_CONNECT",
					"RPCN_ERROR_LOGIN_ERROR",
					"RPCN_ERROR_ALREADY_LOGGED",
					"RPCN_ERROR_INVALID_LOGIN",
					"RPCN_ERROR_INVALID_PASSWORD",
					"RPCN_ERROR_INVALID_TOKEN",
					"RPCN_ERROR_INVALID_PROTOCOL_VERSION",
					"RPCN_ERROR_UNKNOWN",
					"RPCN_SUCCESS_LOGGED_ON",
					"RPCN_FRIEND_REQUEST_RECEIVED",
					"RPCN_FRIEND_ADDED",
					"RPCN_FRIEND_LOST",
					"RPCN_FRIEND_LOGGED_IN",
					"RPCN_FRIEND_LOGGED_OUT",

					"HOME_MENU_TITLE",
					"HOME_MENU_EXIT_GAME",
					"HOME_MENU_RESTART",
					"HOME_MENU_RESUME",
					"HOME_MENU_FRIENDS",
					"HOME_MENU_FRIENDS_REQUESTS",
					"HOME_MENU_FRIENDS_BLOCKED",
					"HOME_MENU_FRIENDS_STATUS_ONLINE",
					"HOME_MENU_FRIENDS_STATUS_OFFLINE",
					"HOME_MENU_FRIENDS_STATUS_BLOCKED",
					"HOME_MENU_FRIENDS_REQUEST_SENT",
					"HOME_MENU_FRIENDS_REQUEST_RECEIVED",
					"HOME_MENU_FRIENDS_BLOCK_USER_MSG",
					"HOME_MENU_FRIENDS_UNBLOCK_USER_MSG",
					"HOME_MENU_FRIENDS_REMOVE_USER_MSG",
					"HOME_MENU_FRIENDS_ACCEPT_REQUEST_MSG",
					"HOME_MENU_FRIENDS_CANCEL_REQUEST_MSG",
					"HOME_MENU_FRIENDS_REJECT_REQUEST_MSG",
					"HOME_MENU_FRIENDS_REJECT_REQUEST",
					"HOME_MENU_FRIENDS_NEXT_LIST",
					"HOME_MENU_SETTINGS",
					"HOME_MENU_SETTINGS_SAVE",
					"HOME_MENU_SETTINGS_SAVE_BUTTON",
					"HOME_MENU_SETTINGS_DISCARD",
					"HOME_MENU_SETTINGS_DISCARD_BUTTON",
					"HOME_MENU_SETTINGS_AUDIO",
					"HOME_MENU_SETTINGS_AUDIO_MASTER_VOLUME",
					"HOME_MENU_SETTINGS_AUDIO_BACKEND",
					"HOME_MENU_SETTINGS_AUDIO_BUFFERING",
					"HOME_MENU_SETTINGS_AUDIO_BUFFER_DURATION",
					"HOME_MENU_SETTINGS_AUDIO_TIME_STRETCHING",
					"HOME_MENU_SETTINGS_AUDIO_TIME_STRETCHING_THRESHOLD",
					"HOME_MENU_SETTINGS_VIDEO",
					"HOME_MENU_SETTINGS_VIDEO_FRAME_LIMIT",
					"HOME_MENU_SETTINGS_VIDEO_ANISOTROPIC_OVERRIDE",
					"HOME_MENU_SETTINGS_VIDEO_OUTPUT_SCALING",
					"HOME_MENU_SETTINGS_VIDEO_RCAS_SHARPENING",
					"HOME_MENU_SETTINGS_VIDEO_STRETCH_TO_DISPLAY",
					"HOME_MENU_SETTINGS_INPUT",
					"HOME_MENU_SETTINGS_INPUT_BACKGROUND_INPUT",
					"HOME_MENU_SETTINGS_INPUT_KEEP_PADS_CONNECTED",
					"HOME_MENU_SETTINGS_INPUT_SHOW_PS_MOVE_CURSOR",
					"HOME_MENU_SETTINGS_INPUT_CAMERA_FLIP",
					"HOME_MENU_SETTINGS_INPUT_PAD_MODE",
					"HOME_MENU_SETTINGS_INPUT_PAD_SLEEP",
					"HOME_MENU_SETTINGS_INPUT_FAKE_MOVE_ROTATION_CONE_H",
					"HOME_MENU_SETTINGS_INPUT_FAKE_MOVE_ROTATION_CONE_V",
					"HOME_MENU_SETTINGS_ADVANCED",
					"HOME_MENU_SETTINGS_ADVANCED_PREFERRED_SPU_THREADS",
					"HOME_MENU_SETTINGS_ADVANCED_MAX_CPU_PREEMPTIONS",
					"HOME_MENU_SETTINGS_ADVANCED_ACCURATE_RSX_RESERVATION_ACCESS",
					"HOME_MENU_SETTINGS_ADVANCED_SLEEP_TIMERS_ACCURACY",
					"HOME_MENU_SETTINGS_ADVANCED_MAX_SPURS_THREADS",
					"HOME_MENU_SETTINGS_ADVANCED_DRIVER_WAKE_UP_DELAY",
					"HOME_MENU_SETTINGS_ADVANCED_VBLANK_FREQUENCY",
					"HOME_MENU_SETTINGS_ADVANCED_VBLANK_NTSC",
					"HOME_MENU_SETTINGS_OVERLAYS",
					"HOME_MENU_SETTINGS_OVERLAYS_SHOW_TROPHY_POPUPS",
					"HOME_MENU_SETTINGS_OVERLAYS_SHOW_RPCN_POPUPS",
					"HOME_MENU_SETTINGS_OVERLAYS_SHOW_SHADER_COMPILATION_HINT",
					"HOME_MENU_SETTINGS_OVERLAYS_SHOW_PPU_COMPILATION_HINT",
					"HOME_MENU_SETTINGS_OVERLAYS_SHOW_AUTO_SAVE_LOAD_HINT",
					"HOME_MENU_SETTINGS_OVERLAYS_SHOW_PRESSURE_INTENSITY_TOGGLE_HINT",
					"HOME_MENU_SETTINGS_OVERLAYS_SHOW_ANALOG_LIMITER_TOGGLE_HINT",
					"HOME_MENU_SETTINGS_OVERLAYS_SHOW_MOUSE_AND_KB_TOGGLE_HINT",
					"HOME_MENU_SETTINGS_PERFORMANCE_OVERLAY",
					"HOME_MENU_SETTINGS_PERFORMANCE_OVERLAY_ENABLE",
					"HOME_MENU_SETTINGS_PERFORMANCE_OVERLAY_ENABLE_FRAMERATE_GRAPH",
					"HOME_MENU_SETTINGS_PERFORMANCE_OVERLAY_ENABLE_FRAMETIME_GRAPH",
					"HOME_MENU_SETTINGS_PERFORMANCE_OVERLAY_DETAIL_LEVEL",
					"HOME_MENU_SETTINGS_PERFORMANCE_OVERLAY_FRAMERATE_DETAIL_LEVEL",
					"HOME_MENU_SETTINGS_PERFORMANCE_OVERLAY_FRAMETIME_DETAIL_LEVEL",
					"HOME_MENU_SETTINGS_PERFORMANCE_OVERLAY_FRAMERATE_DATAPOINT_COUNT",
					"HOME_MENU_SETTINGS_PERFORMANCE_OVERLAY_FRAMETIME_DATAPOINT_COUNT",
					"HOME_MENU_SETTINGS_PERFORMANCE_OVERLAY_UPDATE_INTERVAL",
					"HOME_MENU_SETTINGS_PERFORMANCE_OVERLAY_POSITION",
					"HOME_MENU_SETTINGS_PERFORMANCE_OVERLAY_CENTER_X",
					"HOME_MENU_SETTINGS_PERFORMANCE_OVERLAY_CENTER_Y",
					"HOME_MENU_SETTINGS_PERFORMANCE_OVERLAY_MARGIN_X",
					"HOME_MENU_SETTINGS_PERFORMANCE_OVERLAY_MARGIN_Y",
					"HOME_MENU_SETTINGS_PERFORMANCE_OVERLAY_FONT_SIZE",
					"HOME_MENU_SETTINGS_PERFORMANCE_OVERLAY_OPACITY",
					"HOME_MENU_SETTINGS_DEBUG",
					"HOME_MENU_SETTINGS_DEBUG_OVERLAY",
					"HOME_MENU_SETTINGS_DEBUG_INPUT_OVERLAY",
					"HOME_MENU_SETTINGS_DEBUG_DISABLE_VIDEO_OUTPUT",
					"HOME_MENU_SETTINGS_DEBUG_TEXTURE_LOD_BIAS",
					"HOME_MENU_SCREENSHOT",
					"HOME_MENU_SAVESTATE",
					"HOME_MENU_SAVESTATE_SAVE",
					"HOME_MENU_SAVESTATE_AND_EXIT",
					"HOME_MENU_RELOAD_SAVESTATE",
					"HOME_MENU_RECORDING",
					"HOME_MENU_TROPHIES",
					"HOME_MENU_TROPHY_LIST_TITLE",
					"HOME_MENU_TROPHY_LOCKED_TITLE",
					"HOME_MENU_TROPHY_HIDDEN_TITLE",
					"HOME_MENU_TROPHY_HIDDEN_DESCRIPTION",
					"HOME_MENU_TROPHY_PLATINUM_RELEVANT",
					"HOME_MENU_TROPHY_GRADE_BRONZE",
					"HOME_MENU_TROPHY_GRADE_SILVER",
					"HOME_MENU_TROPHY_GRADE_GOLD",
					"HOME_MENU_TROPHY_GRADE_PLATINUM",

					"PROGRESS_DIALOG_PROGRESS",
					"PROGRESS_DIALOG_PROGRESS_ANALYZING",
					"PROGRESS_DIALOG_REMAINING",
					"PROGRESS_DIALOG_DONE",
					"PROGRESS_DIALOG_FILE",
					"PROGRESS_DIALOG_MODULE",
					"PROGRESS_DIALOG_OF",
					"PROGRESS_DIALOG_PLEASE_WAIT",
					"PROGRESS_DIALOG_STOPPING_PLEASE_WAIT",
					"PROGRESS_DIALOG_SCANNING_PPU_EXECUTABLE",
					"PROGRESS_DIALOG_ANALYZING_PPU_EXECUTABLE",
					"PROGRESS_DIALOG_SCANNING_PPU_MODULES",
					"PROGRESS_DIALOG_LOADING_PPU_MODULES",
					"PROGRESS_DIALOG_COMPILING_PPU_MODULES",
					"PROGRESS_DIALOG_LINKING_PPU_MODULES",
					"PROGRESS_DIALOG_APPLYING_PPU_CODE",
					"PROGRESS_DIALOG_BUILDING_SPU_CACHE",
				"PROGRESS_DIALOG_PRELOADING_SHADER_CACHE",

					"EMULATION_PAUSED_RESUME_WITH_START",
					"EMULATION_RESUMING",
					"EMULATION_FROZEN",
					"SAVESTATE_FAILED_DUE_TO_VDEC",
					"SAVESTATE_FAILED_DUE_TO_SAVEDATA",
					"SAVESTATE_FAILED_DUE_TO_SPU",
					"SAVESTATE_FAILED_DUE_TO_MISSING_SPU_SETTING",
		};

		for(String k:localized_string_keys){
			try {
				int resId = app.getResources().getIdentifier(k, "string", app.getPackageName());
				if (resId != 0) {
					aenu.lang.System.setenv(k, app.getString(resId));
				} else {
					throw new RuntimeException("String resource not found: " + k);
				}
			} catch (Exception e) {
				throw new RuntimeException(e.getMessage());
			}
		}
	}

	public boolean support_custom_driver(){
		try {
			return new File("/dev/kgsl-3d0").exists();
		}catch(Exception e){
			return false;
		}
	}

	 public native String[] get_support_llvm_cpu_list();

	public native String[] get_vulkan_physical_dev_list();

	//public native boolean support_custom_driver();
	public native String generate_config_xml();
	public native String generate_strings_xml();
	public native String generate_java_string_arr();
	//public native MetaInfo meta_info_from_iso(String p) throws RuntimeException;
	
	public native MetaInfo meta_info_from_dir(String p) throws RuntimeException;
	public native MetaInfo meta_info_from_iso(int fd,String iso_uri) throws RuntimeException;
	public native boolean install_firmware(int pup_file_fd);

	public boolean install_rap(int pfd,String rap_name){
		File rap_dir=new File(Application.get_app_data_dir(),"config/dev_hdd0/home/00000001/exdata");
		ParcelFileDescriptor rap_file_in=ParcelFileDescriptor.adoptFd(pfd);
		if(!rap_dir.exists()) rap_dir.mkdirs();
		File rap_file_out=new File(rap_dir,rap_name);
        try {
			FileInputStream rap_file_in_s=new FileInputStream(rap_file_in.getFileDescriptor());
			if(rap_file_in_s.available()!=16){
				rap_file_in_s.close();
				rap_file_in.close();
				return false;
			}
            FileOutputStream rap_file_out_s=new FileOutputStream(rap_file_out);
			int n;
			while((n=rap_file_in_s.read())!=-1)
				rap_file_out_s.write(n);
			rap_file_in_s.close();
			rap_file_out_s.close();
			rap_file_in.close();
        } catch (Exception e) {
            return false;
        }
        return true;
	}
	public native boolean install_pkg(int pkg_fd);
	
	//public native boolean inatall_iso(String iso_path,String game_dir);
	public native void setup_game_info(MetaInfo info);
	public native void setup_surface(Surface sf);
	public native void boot() throws BootException;
	
	public native void key_event(int key_code,boolean pressed,int value);

	public native void quit();

	public native boolean is_running();
	public native boolean is_paused();
	
	public native void pause();
	
	public native void resume();
    
    public native void change_surface(int w,int h);

    public void key_event(int key_code,boolean pressed){
        key_event(key_code,pressed,255);
    }

	public native String simple_device_info();
    
}
