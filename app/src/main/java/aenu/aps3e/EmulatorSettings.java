package aenu.aps3e;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.Typeface;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.ParcelFileDescriptor;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.style.ForegroundColorSpan;
import android.text.style.StrikethroughSpan;
import android.text.style.StyleSpan;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.activity.OnBackPressedCallback;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.DialogFragment;

import androidx.preference.CheckBoxPreference;
import androidx.preference.ListPreference;
import androidx.preference.ListPreferenceDialogFragmentCompat;
import androidx.preference.Preference;
import androidx.preference.PreferenceDataStore;
import androidx.preference.PreferenceFragmentCompat;
import androidx.preference.PreferenceScreen;
import androidx.preference.SeekBarPreference;


import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import aenu.preference.ColorPickerDialog;

public class EmulatorSettings extends AppCompatActivity {

    static final int REQUEST_CODE_SELECT_CUSTOM_DRIVER=6001;
    static final int REQUEST_CODE_SELECT_CUSTOM_FONT=6002;

    @SuppressLint("ValidFragment")
    public static class SettingsFragment extends PreferenceFragmentCompat implements
            Preference.OnPreferenceClickListener{

        Emulator.Config config;
        PreferenceScreen root_pref;

        OnBackPressedCallback back_callback=new OnBackPressedCallback(true) {
            @Override
            public void handleOnBackPressed() {
                String current=SettingsFragment.this.getPreferenceScreen().getKey();
                if (current==null){
                    requireActivity().finish();
                    return;
                }
                int p=current.lastIndexOf('|');
                if (p==-1)
                    setPreferenceScreen(root_pref);
                else
                    setPreferenceScreen(root_pref.findPreference(current.substring(0,p)));
            }
        };

        final PreferenceDataStore data_store=new PreferenceDataStore(){

            public void putString(String key, @Nullable String value) {
                config.save_config_entry(key,value);
            }

            public void putStringSet(String key, @Nullable Set<String> values) {
                throw new UnsupportedOperationException("Not implemented on this data store");
            }

            public void putInt(String key, int value) {
                config.save_config_entry(key,Integer.toString(value));
            }

            public void putLong(String key, long value) {
                throw new UnsupportedOperationException("Not implemented on this data store");
            }

            public void putFloat(String key, float value) {
                throw new UnsupportedOperationException("Not implemented on this data store");
            }

            public void putBoolean(String key, boolean value) {
                config.save_config_entry(key,Boolean.toString(value));
            }

            @Nullable
            public String getString(String key, @Nullable String defValue) {
                return config.load_config_entry(key);
            }

            @Nullable
            public Set<String> getStringSet(String key, @Nullable Set<String> defValues) {
                //return defValues;
                throw new UnsupportedOperationException("Not implemented on this data store");
            }

            public int getInt(String key, int defValue) {
                return Integer.parseInt(config.load_config_entry(key));
            }

            public long getLong(String key, long defValue) {
                throw new UnsupportedOperationException("Not implemented on this data store");
            }

            public float getFloat(String key, float defValue) {
                throw new UnsupportedOperationException("Not implemented on this data store");
            }

            public boolean getBoolean(String key, boolean defValue) {
                return Boolean.parseBoolean(config.load_config_entry(key));
            }
        };

        Preference reset_as_default_pref(){
            Preference p=new Preference(requireContext());
            p.setTitle(R.string.reset_as_default);
            p.setOnPreferenceClickListener(new Preference.OnPreferenceClickListener(){
                public boolean onPreferenceClick(@NonNull Preference preference) {
                    new AlertDialog.Builder(requireContext())
                            .setMessage(getString(R.string.reset_as_default)+"?")
                            .setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int which) {
                                    if (config!=null) {
                                        config.close_config_file();
                                        config=null;
                                    }
                                    MainActivity.get_config_file().delete();
                                    Application.extractAssetsDir(requireContext(),"config",MainActivity.get_config_file().getParentFile());
                                    requireActivity().finish();
                                }
                            })
                            .setNegativeButton(android.R.string.cancel, null)
                            .create().show();
                    return true;
                }
            });
            return p;
        }

        public void setPreferenceScreen(PreferenceScreen preferenceScreen){
            super.setPreferenceScreen(preferenceScreen);
            CharSequence title=preferenceScreen.getTitle();
            if(title==null)
                title=getString(R.string.settings);
            requireActivity().setTitle(title);
        }

        @Override
        public void onCreatePreferences(@Nullable Bundle savedInstanceState, @Nullable String rootKey) {

            if(rootKey!=null) throw new RuntimeException();

            setPreferencesFromResource(R.xml.emulator_settings, rootKey);
            root_pref=getPreferenceScreen();

            root_pref.addPreference(reset_as_default_pref());

            requireActivity().getOnBackPressedDispatcher().addCallback(back_callback);

            if(!MainActivity.get_config_file().exists()){
                root_pref.setEnabled(false);
                return;
            }

            try{
                config=Emulator.Config.open_config_file(MainActivity.get_config_file().getAbsolutePath());
            }catch(Exception e){
                Log.e("EmulatorSettings",e.toString());
                root_pref.setEnabled(false);
                return;
            }





            final String[] BOOL_KEYS={
                    "Core|PPU Debug",
                    "Core|PPU Calling History",
                    "Core|Save LLVM logs",
                    "Core|LLVM Precompilation",
                    "Core|Set DAZ and FTZ",
                    "Core|Disable SPU GETLLAR Spin Optimization",
                    "Core|SPU Debug",
                    "Core|MFC Debug",
                    "Core|SPU loop detection",
                    "Core|Accurate SPU DMA",
                    "Core|Accurate SPU Reservations",
                    "Core|Accurate Cache Line Stores",
                    "Core|Accurate RSX reservation access",
                    "Core|SPU Verification",
                    "Core|SPU Cache",
                    "Core|SPU Profiler",
                    "Core|MFC Commands Shuffling In Steps",
                    "Core|Precise SPU Verification",
                    "Core|PPU LLVM Java Mode Handling",
                    "Core|Use Accurate DFMA",
                    "Core|PPU Set Saturation Bit",
                    "Core|PPU Accurate Non-Java Mode",
                    "Core|PPU Fixup Vector NaN Values",
                    "Core|PPU Accurate Vector NaN Values",
                    "Core|PPU Set FPCC Bits",
                    "Core|Debug Console Mode",
                    "Core|Hook static functions",
                    "Core|HLE lwmutex",
                    "Core|Allow RSX CPU Preemptions",
                    "Core|Enable Performance Report",
                    "Core|Assume External Debugger",
                    "Video|Write Color Buffers",
                    "Video|Write Depth Buffer",
                    "Video|Read Color Buffers",
                    "Video|Read Depth Buffer",
                    "Video|Handle RSX Memory Tiling",
                    "Video|Log shader programs",
                    "Video|VSync",
                    "Video|Debug output",
                    "Video|Debug overlay",
                    "Video|Renderdoc Compatibility Mode",
                    "Video|Use GPU texture scaling",
                    "Video|Stretch To Display Area",
                    "Video|Force High Precision Z buffer",
                    "Video|Strict Rendering Mode",
                    "Video|Disable ZCull Occlusion Queries",
                    "Video|Disable Video Output",
                    "Video|Disable Vertex Cache",
                    "Video|Disable FIFO Reordering",
                    "Video|Enable Frame Skip",
                    "Video|Force CPU Blit",
                    "Video|Disable On-Disk Shader Cache",
                    "Video|Disable Vulkan Memory Allocator",
                    "Video|Use full RGB output range",
                    "Video|Strict Texture Flushing",
                    "Video|Multithreaded RSX",
                    "Video|Relaxed ZCULL Sync",
                    "Video|Force Hardware MSAA Resolve",
                    "Video|Debug Program Analyser",
                    "Video|Accurate ZCULL stats",
                    "Video|Vblank NTSC Fixup",
                    "Video|DECR memory layout",
                    "Video|Allow Host GPU Labels",
                    "Video|Disable Asynchronous Memory Manager",
                    "Video|Vulkan|Force FIFO present mode",
                    "Video|Vulkan|Asynchronous Texture Streaming 2",
                    "Video|Vulkan|Use Custom Driver",
                    "Video|Vulkan|Custom Driver Force Max Clocks",
                    "Video|Performance Overlay|Enabled",
                    "Video|Performance Overlay|Enable Framerate Graph",
                    "Video|Performance Overlay|Enable Frametime Graph",
                    "Video|Performance Overlay|Center Horizontally",
                    "Video|Performance Overlay|Center Vertically",
                    "Video|Shader Loading Dialog|Allow custom background",
                    "Audio|Dump to file",
                    "Audio|Convert to 16 bit",
                    "Audio|Enable Buffering",
                    "Audio|Enable Time Stretching",
                    "Audio|Disable Sampling Skip",
                    "Input/Output|Keep pads connected",
                    "Input/Output|Background input enabled",
                    "Input/Output|Show move cursor",
                    "Input/Output|Lock overlay input to player one",
                    "Input/Output|Load SDL GameController Mappings",
                    "Input/Output|IO Debug overlay",
                    "Savestate|Start Paused",
                    "Savestate|Suspend Emulation Savestate Mode",
                    "Savestate|Compatible Savestate Mode",
                    "Savestate|Inspection Mode Savestates",
                    "Savestate|Save Disc Game Data",
                    "Miscellaneous|Automatically start games after boot",
                    "Miscellaneous|Exit RPCS3 when process finishes",
                    "Miscellaneous|Pause emulation on RPCS3 focus loss",
                    "Miscellaneous|Start games in fullscreen mode",
                    "Miscellaneous|Show trophy popups",
                    "Miscellaneous|Show RPCN popups",
                    "Miscellaneous|Show shader compilation hint",
                    "Miscellaneous|Show PPU compilation hint",
                    "Miscellaneous|Show pressure intensity toggle hint",
                    "Miscellaneous|Show analog limiter toggle hint",
                    "Miscellaneous|Show autosave/autoload hint",
                    "Miscellaneous|Use native user interface",
                    "Miscellaneous|Silence All Logs",
                    "Miscellaneous|Pause Emulation During Home Menu",
            };
            final String[] INT_KEYS={
                    "Core|PPU Threads",
                    "Core|Max LLVM Compile Threads",
                    "Core|Preferred SPU Threads",
                    "Core|SPU delay penalty",
                    "Core|Max SPURS Threads",
                    "Core|Accurate PPU 128-byte Reservation Op Max Length",
                    "Core|Stub PPU Traps",
                    "Core|Clocks scale",
                    "Core|Usleep Time Addend",
                    "Video|Second Frame Limit",
                    "Video|Consecutive Frames To Draw",
                    "Video|Consecutive Frames To Skip",
                    "Video|Resolution Scale",
                    "Video|Texture LOD Bias Addend",
                    "Video|Minimum Scalable Dimension",
                    "Video|Shader Compiler Threads",
                    "Video|Driver Recovery Timeout",
                    "Video|Vblank Rate",
                    "Audio|Master Volume",
                    "Audio|Desired Audio Buffer Duration",
                    "Audio|Time Stretching Threshold",
                    "Core|SPU Reservation Busy Waiting Percentage",
                    "Core|SPU GETLLAR Busy Waiting Percentage",
                    "Core|MFC Commands Shuffling Limit",
                    "Core|MFC Commands Timeout",
                    "Core|TSX Transaction First Limit",
                    "Core|TSX Transaction Second Limit",
                    "Core|SPU Wake-Up Delay",
                    "Core|SPU Wake-Up Delay Thread Mask",
                    "Core|Max CPU Preempt Count",
                    "Core|Performance Report Threshold",
                    "Video|Anisotropic Filter Override",
                    "Video|Driver Wake-Up Delay",
                    "Video|Vulkan|FidelityFX CAS Sharpening Intensity",
                    "Video|Vulkan|VRAM allocation limit (MB)",
                    "Video|Performance Overlay|Framerate datapoints",
                    "Video|Performance Overlay|Frametime datapoints",
                    "Video|Performance Overlay|Metrics update interval (ms)",
                    "Video|Performance Overlay|Font size (px)",
                    "Video|Performance Overlay|Horizontal Margin (px)",
                    "Video|Performance Overlay|Vertical Margin (px)",
                    "Video|Performance Overlay|Opacity (%)",
                    "Video|Shader Loading Dialog|Darkening effect strength",
                    "Video|Shader Loading Dialog|Blur effect strength",
                    "Audio|Audio Formats",
                    "Input/Output|Pad handler sleep (microseconds)",
            };
            final String[] STRING_ARR_KEYS={
                    "Core|PPU Decoder",
                    "Core|Thread Scheduler Mode",
                    "Core|SPU Decoder",
                    "Core|SPU Block Size",
                    "Core|RSX FIFO Accuracy",
                    "Core|Enable TSX",
                    "Core|XFloat Accuracy",
                    "Core|Sleep Timers Accuracy",
                    "Video|Renderer",
                    "Video|Resolution",
                    "Video|Aspect ratio",
                    "Video|Frame limit",
                    "Video|MSAA",
                    "Video|Shader Mode",
                    "Video|Shader Precision",
                    "Video|3D Display Mode",
                    "Video|Output Scaling Mode",
                    "Video|Vertex Buffer Upload Mode",
                    "Video|Vulkan|Exclusive Fullscreen Mode",
                    "Video|Vulkan|Asynchronous Queue Scheduler",
                    "Video|Performance Overlay|Detail level",
                    "Video|Performance Overlay|Framerate graph detail level",
                    "Video|Performance Overlay|Frametime graph detail level",
                    "Video|Performance Overlay|Position",
                    "Audio|Renderer",
                    "Audio|Audio Provider",
                    "Audio|RSXAudio Avport",
                    "Audio|Audio Format",
                    "Audio|Audio Channel Layout",
                    "Audio|Microphone Type",
                    "Audio|Music Handler",
                    "Input/Output|Keyboard",
                    "Input/Output|Mouse",
                    "Input/Output|Camera",
                    "Input/Output|Camera type",
                    "Input/Output|Camera flip",
                    "Input/Output|Move",
                    "Input/Output|Buzz emulated controller",
                    "Input/Output|Turntable emulated controller",
                    "Input/Output|GHLtar emulated controller",
                    "Input/Output|Pad handler mode",
                    "System|License Area",
                    "System|Language",
                    "System|Keyboard Type",
                    "System|Enter button assignment",
                    "Miscellaneous|Font File Selection",
            };
            final String[] NODE_KEYS={
                    "Core",
                    "Video",
                    "Video|Vulkan",
                    "Video|Performance Overlay",
                    "Video|Shader Loading Dialog",
                    "Audio",
                    "Input/Output",
                    "System",
                    "Savestate",
                    "Miscellaneous",
            };


            for (String key:BOOL_KEYS){
                CheckBoxPreference pref=findPreference(key);
                String val_str=config.load_config_entry(key);
                if (val_str!=null) {
                    boolean val=Boolean.parseBoolean(val_str);
                    pref.setChecked(val);
                }
                //pref.setOnPreferenceChangeListener(this);
                pref.setPreferenceDataStore(data_store);
            }

            for (String key:INT_KEYS){
                SeekBarPreference pref=findPreference(key);
                String val_str=config.load_config_entry(key);
                if (val_str!=null) {
                    //FIXME
                    try {
                        int val = Integer.parseInt(val_str);
                        pref.setValue(val);
                    } catch (NumberFormatException e) {
                        pref.setEnabled(false);
                    }
                }

                //pref.setOnPreferenceChangeListener(this);
                pref.setPreferenceDataStore(data_store);
            }

            final Preference.OnPreferenceChangeListener list_pref_change_listener=new Preference.OnPreferenceChangeListener() {
                @Override
                public boolean onPreferenceChange(@NonNull Preference preference, Object newValue) {
                    ListPreference pref=(ListPreference) preference;
                    CharSequence value=(CharSequence) newValue;
                    CharSequence[] values=pref.getEntryValues();
                    CharSequence[] entries=pref.getEntries();
                    for (int i=0;i<values.length;i++){
                        if (values[i].equals(value)){
                            pref.setSummary(entries[i]);
                            break;
                        }
                    }
                    return true;
                }
            };
            for (String key:STRING_ARR_KEYS){
                ListPreference pref=findPreference(key);
                String val_str=config.load_config_entry(key);
                if (val_str!=null) {
                    pref.setValue(val_str);
                    pref.setSummary(pref.getEntry());
                }
                pref.setOnPreferenceChangeListener(list_pref_change_listener);
                pref.setPreferenceDataStore(data_store);
            }

            for (String key:NODE_KEYS){
                PreferenceScreen pref=findPreference(key);
                pref.setOnPreferenceClickListener(this);
            }

            findPreference("Core|Libraries Control").setOnPreferenceClickListener(this);

            String val;
            findPreference("Core|Use LLVM CPU").setOnPreferenceClickListener(this);
            if((val=config.load_config_entry("Core|Use LLVM CPU"))!=null) findPreference("Core|Use LLVM CPU").setSummary(val);

            findPreference("Video|Vulkan|Adapter").setOnPreferenceClickListener(this);
            if((val=config.load_config_entry("Video|Vulkan|Adapter"))!=null) findPreference("Video|Vulkan|Adapter").setSummary(val);

            findPreference("Video|Vulkan|Custom Driver Library Path").setOnPreferenceClickListener(this);
            findPreference("Miscellaneous|Custom Font File Path").setOnPreferenceClickListener(this);

            setup_costom_driver_library_path(null);
            setup_costom_font_path(null);

            if(!Emulator.get.support_custom_driver()){
                findPreference("Video|Vulkan|Use Custom Driver").setEnabled(false);
                findPreference("Video|Vulkan|Custom Driver Library Path").setEnabled(false);
                findPreference("Video|Vulkan|Custom Driver Force Max Clocks").setEnabled(false);
                //return;
            }

            //Android 15+
            if(Build.VERSION.SDK_INT>=35){
                /*ListPreference pref=findPreference("Miscellaneous|Font File Selection");
                String entry_values[]=getResources().getStringArray(R.array.miscellaneous_font_file_selection_values);
                String entries[]=getResources().getStringArray(R.array.miscellaneous_font_file_selection_entries);

                int remove_idx=-1;
                for(int i=0;i<entry_values.length;i++){
                    if(entry_values[i].equals("From OS")){
                        remove_idx=i;
                        break;
                    }
                }

                String[] new_entry_values=new String[entry_values.length-1];
                String[] new_entries=new String[entry_values.length-1];
                System.arraycopy(entry_values,0,new_entry_values,0,remove_idx);
                System.arraycopy(entry_values,remove_idx+1,new_entry_values,remove_idx,entry_values.length-remove_idx-1);
                System.arraycopy(entries,0,new_entries,0,remove_idx);
                System.arraycopy(entries,remove_idx+1,new_entries,remove_idx,entry_values.length-remove_idx-1);

                pref.setEntries(new_entries);
                pref.setEntryValues(new_entry_values);*/
            }

        }


    @Override
    public void onDisplayPreferenceDialog( @NonNull Preference pref) {
        if (pref instanceof ColorPickerDialog) {
            final DialogFragment f = ColorPickerDialog.ColorPickerPreferenceFragmentCompat.newInstance(pref.getKey());
            f.setTargetFragment(this, 0);
            f.show(getParentFragmentManager(), "DIALOG_FRAGMENT_TAG");
            return;
        }
        super.onDisplayPreferenceDialog(pref);
    }



        void setup_costom_driver_library_path(String new_path) {
            final String key="Video|Vulkan|Custom Driver Library Path";
            if(new_path==null){
                String val_str=config.load_config_entry(key);
                if (val_str!=null) {
                    findPreference(key).setSummary(val_str);
                }
                return;
            }

            config.save_config_entry(key,new_path);
            findPreference(key).setSummary(new_path);
        }

        void setup_costom_font_path(String new_path) {
            final String key="Miscellaneous|Custom Font File Path";
            if(new_path==null){
                String val_str=config.load_config_entry(key);
                if (val_str!=null) {
                    findPreference(key).setSummary(val_str);
                }
                return;
            }

            config.save_config_entry(key,new_path);
            findPreference(key).setSummary(new_path);
        }

        @Override
        public void onDestroy() {
            super.onDestroy();
            if (config!=null)
                config.close_config_file();
        }

        /*@Override
        public boolean onPreferenceChange(Preference preference, Object newValue) {
            Log.i("onPreferenceChange",preference.getKey()+" "+newValue);
            if (preference instanceof CheckBoxPreference){
                config.save_config_entry(preference.getKey(),newValue.toString());
            }else if (preference instanceof ListPreference){
                config.save_config_entry(preference.getKey(),newValue.toString());
            }else if (preference instanceof SeekBarPreference){
                config.save_config_entry(preference.getKey(),newValue.toString());
            }
            return true;
        }*/

        void request_select_custom_driver_file(){
            Intent intent=new Intent(Intent.ACTION_OPEN_DOCUMENT);
            intent.addCategory(Intent.CATEGORY_OPENABLE);
            intent.setType("*/*");
            ((Activity)requireActivity()).startActivityForResult(intent, REQUEST_CODE_SELECT_CUSTOM_DRIVER);
        }

        void request_select_font_file(){
            Intent intent=new Intent(Intent.ACTION_OPEN_DOCUMENT);
            intent.addCategory(Intent.CATEGORY_OPENABLE);
            intent.setType("*/*");
            ((Activity)requireActivity()).startActivityForResult(intent, REQUEST_CODE_SELECT_CUSTOM_FONT);
        }

        @Override
        public boolean onPreferenceClick(@NonNull Preference preference) {

            if(preference.getKey().equals("Core|Use LLVM CPU")){
                show_select_llvm_cpu_list();
                return false;
            }

            if(preference.getKey().equals("Video|Vulkan|Adapter")){
                show_select_vulkan_phy_dev_list();
                return false;
            }

            if(preference.getKey().equals("Video|Vulkan|Custom Driver Library Path")){
                show_select_custom_driver_list();
                return false;
            }
            if(preference.getKey().equals("Miscellaneous|Custom Font File Path")){
                //request_select_font_file();
                show_select_font_file_list();
                return false;
            }
            if (preference.getKey().equals("Core|Libraries Control")){
                show_library_control_view();
                return false;
            }
            if(preference instanceof PreferenceScreen){
                setPreferenceScreen(root_pref.findPreference(preference.getKey()));
                return false;
            }
            return false;
        }

        void create_list_dialog(String title, String[] items, DialogInterface.OnClickListener listener){
            AlertDialog.Builder builder = new AlertDialog.Builder(getContext());
            builder.setTitle(title)
                    .setItems(items, listener)
                    .setNegativeButton(android.R.string.cancel, null);
            builder.create().show();
        }

        void show_select_llvm_cpu_list(){
            String[] items=Emulator.get.get_support_llvm_cpu_list();
            create_list_dialog(getString(R.string.emulator_settings_core_use_llvm_cpu)
                    , items, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                    config.save_config_entry("Core|Use LLVM CPU",items[which]);
                    findPreference("Core|Use LLVM CPU").setSummary(items[which]);
                }
            });
        }

        void show_select_vulkan_phy_dev_list(){
            String[] items=Emulator.get.get_vulkan_physical_dev_list();
            create_list_dialog(getString(R.string.emulator_settings_video_vulkan_adapter)
                    , items, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                    config.save_config_entry("Video|Vulkan|Adapter",items[which]);
                    findPreference("Video|Vulkan|Adapter").setSummary(items[which]);
                }
                    }
            );
        }

        void show_select_custom_driver_list(){
            File[] files=Application.get_custom_driver_dir().listFiles();
            if(files==null||files.length==0){
                create_list_dialog(getString(R.string.emulator_settings_video_vulkan_custom_driver_library_path_dialog_title)
                        , new String[]{getString(R.string.emulator_settings_video_vulkan_custom_driver_library_path_dialog_add_hint)}, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                        request_select_custom_driver_file();
                    }
                });
                return;
            }

            String  items[]=new String[files.length+1];
            for(int i=0;i<files.length;i++){
                items[i]=files[i].getName();
            }
            items[files.length]=getString(R.string.emulator_settings_video_vulkan_custom_driver_library_path_dialog_add_hint);
            create_list_dialog(getString(R.string.emulator_settings_video_vulkan_custom_driver_library_path_dialog_title), items, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                    if(which==files.length){
                        request_select_custom_driver_file();
                    }else{
                        setup_costom_driver_library_path(files[which].getAbsolutePath());
                    }
                }
            });
        }

        void show_select_font_file_list(){
            File[] files=Application.get_custom_font_dir().listFiles();
            if(files==null||files.length==0){
                create_list_dialog(getString(R.string.emulator_settings_miscellaneous_custom_font_file_path_dialog_title)
                        , new String[]{getString(R.string.emulator_settings_miscellaneous_custom_font_file_path_dialog_add_hint)}, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                        request_select_font_file();
                    }
                });
                return;
            }

            String  items[]=new String[files.length+1];
            for(int i=0;i<files.length;i++){
                items[i]=files[i].getName();
            }
            items[files.length]=getString(R.string.emulator_settings_miscellaneous_custom_font_file_path_dialog_add_hint);
            create_list_dialog(getString(R.string.emulator_settings_miscellaneous_custom_font_file_path_dialog_title), items, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                    if(which==files.length)
                        request_select_font_file();
                        else
                            setup_costom_font_path(files[which].getAbsolutePath());
                }
            });
        }

        void show_library_control_view(){
            LibraryControlAdapter adapter=new LibraryControlAdapter(getContext());
            adapter.set_modify_libs(config.load_config_entry_ty_arr("Core|Libraries Control"));
            ListView view = new ListView(getContext());
            view.setAdapter(adapter);
            view.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                @Override
                public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                    LibraryControlAdapter  adapter= (LibraryControlAdapter) parent.getAdapter();
                    int new_lib_type=adapter.get_lib_type(position)^1;
                    adapter.set_lib_trpe(position,new_lib_type);
                    config.save_config_entry_ty_arr("Core|Libraries Control",adapter.get_modify_libs());
                }
            });

            AlertDialog.Builder builder = new AlertDialog.Builder(getContext());
            builder.setTitle(R.string.emulator_settings_core_libraries_control)
                    .setView(view)
                    .setNegativeButton(android.R.string.cancel, null);
            builder.create().show();
        }

    }

    SettingsFragment fragment;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        //PreferenceFragmentCompat
        fragment=new SettingsFragment();
        getSupportFragmentManager().beginTransaction().replace(android.R.id.content,fragment).commit();
    }

    boolean install_custom_driver_from_zip(Uri uri){
        try {
            ParcelFileDescriptor pfd = getContentResolver().openFileDescriptor(uri, "r");
            FileInputStream fis = new FileInputStream(pfd.getFileDescriptor());
            ZipInputStream zis = new ZipInputStream(fis);
            for (ZipEntry ze = zis.getNextEntry(); ze != null; ze = zis.getNextEntry()) {
                if (ze.getName().endsWith(".so")) {
                    String lib_path=new File(Application.get_custom_driver_dir() , ze.getName()).getAbsolutePath();
                    FileOutputStream lib_os = new FileOutputStream(lib_path);
                    try {
                        byte[] buffer = new byte[16384];
                        int n;
                        while ((n = zis.read(buffer)) != -1)
                            lib_os.write(buffer, 0, n);
                        lib_os.close();
                        fragment.setup_costom_driver_library_path(lib_path);
                        zis.closeEntry();
                        break;
                    } catch (Exception e) {
                        Toast.makeText(this, e.toString(), Toast.LENGTH_SHORT).show();
                    }
                }
                zis.closeEntry();
            }
            zis.close();
            fis.close();
            pfd.close();
            return true;
        }
        catch (Exception e){
            Toast.makeText(this,e.toString(),Toast.LENGTH_SHORT).show();
            return false;
        }
    }

    boolean install_custom_driver_from_lib(Uri uri){
        try {
            ParcelFileDescriptor pfd = getContentResolver().openFileDescriptor(uri, "r");
            FileInputStream lib_is = new FileInputStream(pfd.getFileDescriptor());
            String lib_path=new File(Application.get_custom_driver_dir() , MainActivity.getFileNameFromUri(uri)).getAbsolutePath();
            FileOutputStream lib_os = new FileOutputStream(lib_path);

            byte[] buffer = new byte[16384];
            int n;
            while ((n = lib_is.read(buffer)) != -1)
                lib_os.write(buffer, 0, n);
            lib_os.close();
            fragment.setup_costom_driver_library_path(lib_path);
            lib_is.close();
            pfd.close();
            return true;
        } catch (Exception e) {
            Toast.makeText(this, e.toString(), Toast.LENGTH_SHORT).show();
            return false;
        }
    }

    boolean install_custom_font(Uri uri){
        try {
            ParcelFileDescriptor pfd = getContentResolver().openFileDescriptor(uri, "r");
            FileInputStream font_is = new FileInputStream(pfd.getFileDescriptor());
            String font_path=new File(Application.get_custom_font_dir(), MainActivity.getFileNameFromUri(uri)).getAbsolutePath();
            FileOutputStream font_os = new FileOutputStream(font_path);

            byte[] buffer = new byte[16384];
            int n;
            while ((n = font_is.read(buffer)) != -1)
                font_os.write(buffer, 0, n);
            font_os.close();
            fragment.setup_costom_font_path(font_path);
            font_is.close();
            pfd.close();
            return true;
        } catch (Exception e) {
            Toast.makeText(this, e.toString(), Toast.LENGTH_SHORT).show();
            return false;
        }
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (resultCode != RESULT_OK || data == null) return;

        Uri uri=data.getData();
        String file_name = MainActivity.getFileNameFromUri(uri);

        switch (requestCode){
            case REQUEST_CODE_SELECT_CUSTOM_DRIVER:
                if(file_name.endsWith(".zip"))
                    install_custom_driver_from_zip(uri);
                else if(file_name.endsWith(".so"))
                    install_custom_driver_from_lib(uri);
                break;
                case REQUEST_CODE_SELECT_CUSTOM_FONT:
                    if(file_name.endsWith(".ttf")||file_name.endsWith(".ttc")||file_name.endsWith(".otf"))
                        install_custom_font(uri);
                    break;
        }
    }

    static class LibraryControlAdapter extends BaseAdapter{

        static final int LIB_TYPE_LLE=0;
        static final int LIB_TYPE_HLE=1;

        static final Map<String, Integer> libs=new HashMap<>();

        static {
libs.put("/dev_flash/sys/internal/libfs_utility_init.sprx", 1);
libs.put("libaacenc.sprx", 0);
libs.put("libaacenc_spurs.sprx", 0);
libs.put("libac3dec.sprx", 0);
libs.put("libac3dec2.sprx", 0);
libs.put("libadec.sprx", 1);
libs.put("libadec2.sprx", 0);
libs.put("libadec_internal.sprx", 0);
libs.put("libad_async.sprx", 0);
libs.put("libad_billboard_util.sprx", 0);
libs.put("libad_core.sprx", 0);
libs.put("libapostsrc_mini.sprx", 0);
libs.put("libasfparser2_astd.sprx", 0);
libs.put("libat3dec.sprx", 0);
libs.put("libat3multidec.sprx", 0);
libs.put("libatrac3multi.sprx", 0);
libs.put("libatrac3plus.sprx", 0);
libs.put("libatxdec.sprx", 1);
libs.put("libatxdec2.sprx", 0);
libs.put("libaudio.sprx", 1);
libs.put("libavcdec.sprx", 0);
libs.put("libavcenc.sprx", 0);
libs.put("libavcenc_small.sprx", 0);
libs.put("libavchatjpgdec.sprx", 0);
libs.put("libbeisobmf.sprx", 0);
libs.put("libbemp2sys.sprx", 0);
libs.put("libcamera.sprx", 1);
libs.put("libcelp8dec.sprx", 0);
libs.put("libcelp8enc.sprx", 0);
libs.put("libcelpdec.sprx", 0);
libs.put("libcelpenc.sprx", 0);
libs.put("libddpdec.sprx", 0);
libs.put("libdivxdec.sprx", 0);
libs.put("libdmux.sprx", 0);
libs.put("libdmuxpamf.sprx", 0);
libs.put("libdtslbrdec.sprx", 0);
libs.put("libfiber.sprx", 0);
libs.put("libfont.sprx", 0);
libs.put("libfontFT.sprx", 0);
libs.put("libfreetype.sprx", 0);
libs.put("libfreetypeTT.sprx", 0);
libs.put("libfs.sprx", 0);
libs.put("libfs_155.sprx", 0);
libs.put("libgcm_sys.sprx", 0);
libs.put("libgem.sprx", 1);
libs.put("libgifdec.sprx", 0);
libs.put("libhttp.sprx", 0);
libs.put("libio.sprx", 1);
libs.put("libjpgdec.sprx", 0);
libs.put("libjpgenc.sprx", 0);
libs.put("libkey2char.sprx", 0);
libs.put("libl10n.sprx", 0);
libs.put("liblv2.sprx", 0);
libs.put("liblv2coredump.sprx", 0);
libs.put("liblv2dbg_for_cex.sprx", 0);
libs.put("libm2bcdec.sprx", 0);
libs.put("libm4aacdec.sprx", 0);
libs.put("libm4aacdec2ch.sprx", 0);
libs.put("libm4hdenc.sprx", 0);
libs.put("libm4venc.sprx", 0);
libs.put("libmedi.sprx", 1);
libs.put("libmic.sprx", 1);
libs.put("libmp3dec.sprx", 0);
libs.put("libmp4.sprx", 0);
libs.put("libmpl1dec.sprx", 0);
libs.put("libmvcdec.sprx", 0);
libs.put("libnet.sprx", 0);
libs.put("libnetctl.sprx", 1);
libs.put("libpamf.sprx", 1);
libs.put("libpngdec.sprx", 0);
libs.put("libpngenc.sprx", 0);
libs.put("libresc.sprx", 0);
libs.put("librtc.sprx", 1);
libs.put("librudp.sprx", 0);
libs.put("libsail.sprx", 0);
libs.put("libsail_avi.sprx", 0);
libs.put("libsail_rec.sprx", 0);
libs.put("libsjvtd.sprx", 0);
libs.put("libsmvd2.sprx", 0);
libs.put("libsmvd4.sprx", 0);
libs.put("libspurs_jq.sprx", 0);
libs.put("libsre.sprx", 0);
libs.put("libssl.sprx", 0);
libs.put("libsvc1d.sprx", 0);
libs.put("libsync2.sprx", 0);
libs.put("libsysmodule.sprx", 0);
libs.put("libsysutil.sprx", 1);
libs.put("libsysutil_ap.sprx", 1);
libs.put("libsysutil_authdialog.sprx", 1);
libs.put("libsysutil_avc2.sprx", 1);
libs.put("libsysutil_avconf_ext.sprx", 1);
libs.put("libsysutil_avc_ext.sprx", 1);
libs.put("libsysutil_bgdl.sprx", 1);
libs.put("libsysutil_cross_controller.sprx", 1);
libs.put("libsysutil_dec_psnvideo.sprx", 1);
libs.put("libsysutil_dtcp_ip.sprx", 1);
libs.put("libsysutil_game.sprx", 1);
libs.put("libsysutil_game_exec.sprx", 1);
libs.put("libsysutil_imejp.sprx", 1);
libs.put("libsysutil_misc.sprx", 1);
libs.put("libsysutil_music.sprx", 1);
libs.put("libsysutil_music_decode.sprx", 1);
libs.put("libsysutil_music_export.sprx", 1);
libs.put("libsysutil_np.sprx", 1);
libs.put("libsysutil_np2.sprx", 1);
libs.put("libsysutil_np_clans.sprx", 1);
libs.put("libsysutil_np_commerce2.sprx", 1);
libs.put("libsysutil_np_eula.sprx", 1);
libs.put("libsysutil_np_installer.sprx", 1);
libs.put("libsysutil_np_sns.sprx", 1);
libs.put("libsysutil_np_trophy.sprx", 1);
libs.put("libsysutil_np_tus.sprx", 1);
libs.put("libsysutil_np_util.sprx", 1);
libs.put("libsysutil_oskdialog_ext.sprx", 1);
libs.put("libsysutil_pesm.sprx", 1);
libs.put("libsysutil_photo_decode.sprx", 1);
libs.put("libsysutil_photo_export.sprx", 1);
libs.put("libsysutil_photo_export2.sprx", 1);
libs.put("libsysutil_photo_import.sprx", 1);
libs.put("libsysutil_photo_network_sharing.sprx", 1);
libs.put("libsysutil_print.sprx", 1);
libs.put("libsysutil_rec.sprx", 1);
libs.put("libsysutil_remoteplay.sprx", 1);
libs.put("libsysutil_rtcalarm.sprx", 1);
libs.put("libsysutil_savedata.sprx", 1);
libs.put("libsysutil_savedata_psp.sprx", 1);
libs.put("libsysutil_screenshot.sprx", 1);
libs.put("libsysutil_search.sprx", 1);
libs.put("libsysutil_storagedata.sprx", 1);
libs.put("libsysutil_subdisplay.sprx", 1);
libs.put("libsysutil_syschat.sprx", 1);
libs.put("libsysutil_sysconf_ext.sprx", 1);
libs.put("libsysutil_userinfo.sprx", 1);
libs.put("libsysutil_video_export.sprx", 1);
libs.put("libsysutil_video_player.sprx", 1);
libs.put("libsysutil_video_upload.sprx", 1);
libs.put("libusbd.sprx", 0);
libs.put("libusbpspcm.sprx", 0);
libs.put("libvdec.sprx", 1);
libs.put("libvoice.sprx", 1);
libs.put("libvpost.sprx", 0);
libs.put("libvpost2.sprx", 0);
libs.put("libwmadec.sprx", 0);
        }

        final String[] libs_name=libs.keySet().toArray(new String[0]);

        final Map<String,  Integer> modify=new HashMap<>();

        Context  context;
        LibraryControlAdapter(Context ctx){
            this.context=ctx;
        }

        int get_lib_type(int pos){
            if(modify.containsKey(libs_name[pos]))
                return modify.get(libs_name[pos]);
            return libs.get(libs_name[pos]);
        }

        void set_lib_trpe(int pos,int type){
            int default_type=libs.get(libs_name[pos]);
            if(type==default_type){
                if(modify.containsKey(libs_name[pos]))
                    modify.remove(libs_name[pos]);
            }
            else{
                modify.put(libs_name[pos], type);
            }
            notifyDataSetChanged();
        }

        void set_modify_libs(String[] modify_libs){
            modify.clear();
            if(modify_libs==null||modify_libs.length==0) return;
            for(String s:modify_libs){
                String[] split=s.split(":");
                String lib_name=split[0];
                int type=split[1].equals("lle")?LIB_TYPE_LLE:LIB_TYPE_HLE;
                if(!libs.containsKey(lib_name))
                    continue;
                if(libs.get(lib_name)==type)
                    continue;
                modify.put(lib_name, type);
            }
        }
        String[] get_modify_libs(){
            List<String> l=new ArrayList<>();
            for(Map.Entry<String, Integer> e:modify.entrySet()){
                String lib_name=e.getKey();
                String lib_ty=e.getValue()==LIB_TYPE_LLE?"lle":"hle";
                l.add(lib_name+":"+lib_ty);
            }
            return l.toArray(new String[0]);
        }

        @Override
        public int getCount() {
            return libs_name.length;
        }

        @Override
        public Object getItem(int position) {
            return null;
        }

        @Override
        public long getItemId(int position) {
            return 0;
        }

        LayoutInflater get_inflater(){
            return (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        void setup_type_view(TextView v,int type){
            CharSequence text=type==LIB_TYPE_LLE?"lle":"hle";
            SpannableString span=new SpannableString(text);

            if(type==LIB_TYPE_LLE){
                span.setSpan(new ForegroundColorSpan(Color.YELLOW),0,text.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            }
            else{//HLE
                span.setSpan(new ForegroundColorSpan(Color.BLUE),0,text.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            }
            v.setText(span);
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            if(convertView==null){
                convertView=get_inflater().inflate(R.layout.library_entry,null);
            }
            TextView  name_v=(TextView) convertView.findViewById(R.id.lib_name);
            String name=libs_name[position];
            name_v.setText(name);
            int ty=modify.containsKey(name)?modify.get(name):libs.get(name);
            TextView  type_v=(TextView) convertView.findViewById(R.id.lib_type);
            setup_type_view(type_v,ty);

            return convertView;
        }
    }
}
