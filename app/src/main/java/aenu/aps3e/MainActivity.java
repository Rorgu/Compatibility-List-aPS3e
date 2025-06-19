// SPDX-License-Identifier: WTFPL

package aenu.aps3e;

import android.annotation.TargetApi;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ListActivity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.graphics.Canvas;
import android.graphics.ColorMatrix;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Paint;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Message;
import android.os.ParcelFileDescriptor;
import android.preference.PreferenceManager;
import android.provider.DocumentsContract;
import android.provider.Settings;
import android.text.format.DateFormat;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.FileFilter;
import java.lang.annotation.Target;
import java.text.Collator;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import android.os.Handler;
import java.io.FileOutputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.IOException;
import android.app.Activity;
import android.widget.AdapterView;
import android.widget.Adapter;
import android.Manifest;
import java.util.*;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import aenu.proptest.*;
import java.io.*;
import android.view.*;

import androidx.documentfile.provider.DocumentFile;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class MainActivity extends Activity{

	private static final int REQUEST_INSTALL_FIRMWARE=6001;
	private static final int REQUEST_INSTALL_GAME=6002;
	//private static final int REQUEST_INSTALL_GAME_PKG=6003;
	private static final int REQUEST_SELECT_ISO_DIR=6004;

	private static final String PREF_KEY_ISO_DIR="iso_dir";

	static{System.loadLibrary("e");}

	public static File get_hdd0_game_dir(){
		return new File(Application.get_app_data_dir(),"config/dev_hdd0/game");
	}

	public static File get_game_data_dir(){
		return new File(Application.get_app_data_dir(),"config/dev_hdd0/home/00000001/savedata");
	}

	public static File get_game_trophy_dir(){
		return new File(Application.get_app_data_dir(),"config/dev_hdd0/home/00000001/trophy");
	}

	public static File get_shader_cache_dir(String serial){
		File cache_dir=new File(Application.get_app_data_dir(),"cache/cache/"+serial);
		if(!cache_dir.exists()) return null;
		File[] cache_sub_dir=cache_dir.listFiles();
		if( cache_sub_dir==null||cache_sub_dir.length!=1) return null;
		return new File(cache_sub_dir[0],"shaders_cache");
	}

	public static File get_disc_game_dir(){
		return new File(Application.get_app_data_dir(),"config/games");
	}
	public static File get_config_file(){
		return new File(Application.get_app_data_dir(),"config/config.yml");
	}

	public static File get_game_list_file(){
		return new File(Application.get_app_data_dir(),"config/game_list.json");
	}

	public static File firmware_installed_file(){
		return new File(Application.get_app_data_dir(),"config/dev_flash/.installed");
	}
	
	private final AdapterView.OnItemClickListener item_click_l=new AdapterView.OnItemClickListener(){
		@Override
		public void onItemClick(AdapterView<?> l, View v, int position,long id)
		{
			if(!firmware_installed_file().exists()){
				Toast.makeText(MainActivity.this,getString(R.string.firmware_not_install),Toast.LENGTH_LONG).show();
				return;
			}
			
			Emulator.MetaInfo meta_info=((GameMetaInfoAdapter)l.getAdapter()).getMetaInfo(position);

			if(!meta_info.decrypt) {
				Toast.makeText(MainActivity.this,R.string.undecrypted_game,Toast.LENGTH_LONG).show();
				return;
			}

			Intent intent = new Intent("aenu.intent.action.APS3E");
			intent.setPackage(getPackageName());
			
			intent.putExtra("meta_info",meta_info);
			startActivity(intent);
		}
	};
	
	GameMetaInfoAdapter adapter;

	ProgressTask progress_task;
    @Override
    public void onCreate(Bundle savedInstanceState){

        super.onCreate(savedInstanceState);

		getActionBar().setTitle(getString(R.string.select_game));//"选择游戏");
		android.util.Log.e("aps3e_java","main");

        setContentView(R.layout.activity_main);

		((ListView)findViewById(R.id.game_list)).setOnItemClickListener(item_click_l);
		((ListView)findViewById(R.id.game_list)).setEmptyView(findViewById(R.id.game_list_is_empty));
		//((ListView)findViewById(R.id.game_list)).setOnLongClickListener(item_long_click_l);
		registerForContextMenu(findViewById(R.id.game_list));

		/*boolean is_request_perms_ok;
		if(Build.VERSION.SDK_INT>=30) {
			is_request_perms_ok=check_and_request_perms_API30();
		}
		else{
			is_request_perms_ok=check_and_request_perms();
		}*/


		//if(Build.VERSION.SDK_INT>=30||is_request_perms_ok)
		initialize();

		//if(is_request_perms_ok)
		{
			show_game_list();
		}
    }

	@Override
    public boolean onCreateOptionsMenu(Menu menu){
        super.onCreateOptionsMenu(menu);
        getMenuInflater().inflate(R.menu.main,menu);
		/*if(!getResources().getConfiguration().getLocales().get(0).equals(Locale.SIMPLIFIED_CHINESE))
			menu.removeItem(R.id.menu_update_log);*/
		//menu.removeItem(R.id.menu_set_iso_dir);
        return true;
    }
	
	void request_file_select(int request_code){

		Intent intent=new Intent(Intent.ACTION_OPEN_DOCUMENT);
		intent.addCategory(Intent.CATEGORY_OPENABLE);
		intent.setType("*/*");
		startActivityForResult(intent, request_code);
	}

	void request_iso_dir_select(){
		Intent intent=new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE);
		intent.setFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION|Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION);
		startActivityForResult(intent, REQUEST_SELECT_ISO_DIR);
	}

	void save_pref_iso_dir(Uri uri){
		try{
			getContentResolver().takePersistableUriPermission(uri,Intent.FLAG_GRANT_READ_URI_PERMISSION);
			SharedPreferences.Editor editor=PreferenceManager.getDefaultSharedPreferences(this).edit();
			editor.putString(PREF_KEY_ISO_DIR,uri.toString());
			editor.apply();
		}
		catch(Exception e){
			e.printStackTrace();
		}
	}

	Uri load_pref_iso_dir(){
		try{
			String uri_str=PreferenceManager.getDefaultSharedPreferences(this).getString(PREF_KEY_ISO_DIR,null);
			if(uri_str==null)
				return null;
			Uri uri= Uri.parse(uri_str);
			getContentResolver().takePersistableUriPermission(uri,Intent.FLAG_GRANT_READ_URI_PERMISSION);
			return uri;
		}
		catch(Exception e){
			e.printStackTrace();
			return null;
		}
	}

    @Override
    public boolean onOptionsItemSelected(MenuItem item){

		int item_id=item.getItemId();
		if(item_id==R.id.menu_install_firmware){
			request_file_select(REQUEST_INSTALL_FIRMWARE);
			return true;
		}
		else if(item_id==R.id.menu_install_game){
			request_file_select(REQUEST_INSTALL_GAME);
			return true;
		}
		else if(item_id==R.id.menu_refresh_list){
			refresh_game_list();
			return true;
		}
		else if(item_id==R.id.menu_about){
			startActivity(new Intent(this,HelloJni.class));
			return true;
		}
		/*else if(item_id==R.id.menu_update_log){
			startActivity(new Intent(this,UpdateLogActivity.class));
			return true;
		}*/
		else if(item_id==R.id.menu_key_mappers){
			startActivity(new Intent(this,KeyMapActivity.class));
			return true;
		}
		/*else if(item_id==R.id.menu_gratitude){
			startActivity(new Intent(this,GratitudeActivity.class));
			return true;
		}*/
		else if(item_id==R.id.menu_virtual_pad_edit){
			startActivity(new Intent(this,VirtualPadEdit.class));
			return true;
		}
		else if(item_id==R.id.menu_settings){
			startActivity(new Intent(this,EmulatorSettings.class));
			return true;
		}
		else if(item_id==R.id.menu_open_file_mgr){
			/*Intent it=new Intent(this,TextActivity.class);
			it.putExtra("text",Emulator.get.generate_java_string_arr());
			startActivity(it);*/
			UserDataActivity.open_file_manager(this);
			return true;
		}
		else if(item_id==R.id.menu_set_iso_dir){
			request_iso_dir_select();
			return true;
		}
		return super.onOptionsItemSelected(item);

        /*super.onOptionsItemSelected(item);
        switch(item.getItemId()){
			case R.id.menu_install_firmware:
                request_file_select(FileChooser.REQUEST_INSTALL_FIRMWARE);
                break;    
			case R.id.menu_install_game:
                request_file_select(FileChooser.REQUEST_INSTALL_GAME);
                break;    
			case R.id.menu_refresh_list:
                refresh_game_list();
                break;    
            case R.id.menu_about:
                startActivity(new Intent(this,HelloJni.class));
                break;       
			case R.id.menu_update_log:
				startActivity(new Intent(this,UpdateLogActivity.class));
				break;
			case R.id.menu_key_mappers:
				startActivity(new Intent(this,KeyMapActivity.class));
				break;
			case R.id.menu_gratitude:
				startActivity(new Intent(this,GratitudeActivity.class));
				break;
			case R.id.menu_virtual_pad_edit:
				startActivity(new Intent(this,VirtualPadEdit.class));
				break;
        }
        return true;*/
    }
/*
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		super.onActivityResult(requestCode, resultCode, data);
		if(resultCode!=RESULT_OK) return;
		String path=data.getStringExtra("path");
		switch(requestCode){
			case REQUEST_INSTALL_GAME:{
				if(path.endsWith(".iso")){
				Emulator.MetaInfo mi=Emulator.get.meta_info_from_iso(path);
				if(mi.icon!=null&&mi.serial!=null){
					InstallGame.install_iso(this,path,mi.serial);
				}
				return;}
					else if(path.endsWith(".pkg")){
						InstallGame.install_pkg(this,path,null);
				}
					else if(path.endsWith(".rap")){
						Emulator.get.install_rap(path);
				}
				return ;
			}
			case REQUEST_INSTALL_FIRMWARE:{
				InstallFrimware.start_install_firmware(this,path);
				return;
			}
			default:
			return;
		}
	}*/

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		if (resultCode != RESULT_OK || data == null) return;

		Uri uri = data.getData();

		if(requestCode == REQUEST_SELECT_ISO_DIR){
			save_pref_iso_dir(uri);
			refresh_game_list();
			return;
		}

		if (uri != null) {
			String fileName = getFileNameFromUri(uri);
			int pfd;
            try {
                ParcelFileDescriptor pfd_= getContentResolver().openFileDescriptor(uri, "r");
				pfd=pfd_.detachFd();
				pfd_.close();
            } catch (Exception e) {
				Toast.makeText(this, e.getMessage(), Toast.LENGTH_SHORT).show();
				return;
            }
            switch(requestCode){
				case REQUEST_INSTALL_FIRMWARE:
					//InstallFrimware.start_install_firmware(this,pfd);
					(progress_task=new ProgressTask( this)
							.set_progress_message(getString( R.string.installing_firmware))
							.set_failed_task(new ProgressTask.UI_Task() {
								@Override
								public void run() {
									Toast.makeText(MainActivity.this, getString(R.string.msg_failed), Toast.LENGTH_LONG).show();
								}
							}))
							.call( new ProgressTask.Task() {
								@Override
								public void run(ProgressTask task) {
									if(firmware_installed_file().exists())
										firmware_installed_file().delete();

									if(Emulator.get.install_firmware(pfd)){

                                        try {
                                            firmware_installed_file() .createNewFile();
                                        } catch (IOException e) {
                                            throw new RuntimeException(e);
                                        }

                                        task.task_handler.sendEmptyMessage(ProgressTask.TASK_DONE);
										progress_task=null;
										return;
									}
									task.task_handler.sendEmptyMessage(ProgressTask.TASK_FAILED);
									progress_task=null;
								}
							});
					return;
				case REQUEST_INSTALL_GAME:
					if (fileName.endsWith(".pkg")) {
						/*Log.i("aps3e_java", "installing pkg: "+fileName);
						InstallGame.install_pkg(this, pfd);
						refresh_game_list();*/
						(progress_task=new  ProgressTask(this)
								.set_progress_message(getString(R.string.installing_game))
								.set_failed_task(new ProgressTask.UI_Task() {
									@Override
									public void run() {
										Toast.makeText(MainActivity.this, getString(R.string.msg_failed), Toast.LENGTH_LONG).show();
									}
								}))
								.call(new ProgressTask.Task() {
									@Override
									public void run(ProgressTask task) {
										if (Emulator.get.install_pkg(pfd)) {
											task.task_handler.sendEmptyMessage(ProgressTask.TASK_DONE);
											progress_task = null;
											return;
										}
										task.task_handler.sendEmptyMessage(ProgressTask.TASK_FAILED);
										progress_task = null;
									}
								});
					} else if (fileName.endsWith(".rap")) {
						if(!Emulator.get.install_rap(pfd, fileName)){
							Toast.makeText(this, "Failed to install RAP", Toast.LENGTH_SHORT).show();
						}
					}
					break;
			}
		}
	}

	 static String getFileNameFromUri(Uri uri) {
		String fileName = null;
		Cursor cursor = Application.ctx.getContentResolver().query(
				uri,
				new String[]{DocumentsContract.Document.COLUMN_DISPLAY_NAME},
				null, null, null
		);
		if (cursor != null) {
			if (cursor.moveToFirst()) {
				fileName = cursor.getString(cursor.getColumnIndexOrThrow(
						DocumentsContract.Document.COLUMN_DISPLAY_NAME
				));
			}
			cursor.close();
		}
		return fileName;
	}


	@Override
    public boolean onKeyDown(int keyCode,KeyEvent event){
        if(keyCode==KeyEvent.KEYCODE_BACK){
            finish();
            return true;
        }
        return super.onKeyDown(keyCode,event);
    }
	
	@Override
	public void onCreateContextMenu(ContextMenu menu, View v, ContextMenu.ContextMenuInfo menuInfo) {
		super.onCreateContextMenu(menu, v, menuInfo);
		AdapterView.AdapterContextMenuInfo info = (AdapterView.AdapterContextMenuInfo) menuInfo;
		if(adapter.is_disc_game(info.position)){
			menu.add(0, R.string.delete_hdd0_install_data, 0, getString(R.string.delete_hdd0_install_data));
			menu.add(0, R.string.delete_game_data, 0, getString(R.string.delete_game_data));
			menu.add(0, R.string.delete_shaders_cache, 0, getString(R.string.delete_shaders_cache));
		}
		else{
			menu.add(0, R.string.delete_game_data, 0, getString(R.string.delete_game_data));
			menu.add(0, R.string.delete_game_and_data, 0, getString(R.string.delete_game_and_data));
			menu.add(0, R.string.delete_shaders_cache, 0, getString(R.string.delete_shaders_cache));
		}
	}
	
	@Override
	public boolean onContextItemSelected(MenuItem item) {
		AdapterView.AdapterContextMenuInfo info = (AdapterView.AdapterContextMenuInfo) item.getMenuInfo();
		
		int position = info.position;
		int item_id = item.getItemId();
		if(item_id==R.string.delete_hdd0_install_data){
			show_verify_dialog(R.string.delete_hdd0_install_data, new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
					(progress_task=new ProgressTask(MainActivity.this)).call(new ProgressTask.Task() {
						@Override
						public void run(ProgressTask task) {
							adapter.del_hdd0_install_data(position);
							task.task_handler.sendEmptyMessage(ProgressTask.TASK_DONE);
							progress_task=null;
						}
					});
				}
			});
			return true;
		}
		else if(item_id==R.string.delete_game_data){
			show_verify_dialog(R.string.delete_game_data, new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
					(progress_task=new ProgressTask(MainActivity.this)).call(new ProgressTask.Task() {
						@Override
						public void run(ProgressTask task) {
							adapter.del_game_data(position);
							task.task_handler.sendEmptyMessage(ProgressTask.TASK_DONE);
							progress_task=null;
						}
					});
				}
			});
		}
		else if(item_id==R.string.delete_game_and_data){
			show_verify_dialog(R.string.delete_game_and_data, new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
					(progress_task=new ProgressTask(MainActivity.this)
							.set_done_task(new ProgressTask.UI_Task(){
								@Override
								public void run() {
									refresh_game_list();
								}
							}))
							.call(new ProgressTask.Task() {
						@Override
						public void run(ProgressTask task) {
							adapter.del_hdd0_game_and_data(position);
							task.task_handler.sendEmptyMessage(ProgressTask.TASK_DONE);
							progress_task=null;
						}
					});
				}
			});
		}
		else if(item_id==R.string.delete_shaders_cache){
			(progress_task=new ProgressTask(MainActivity.this)).call(new ProgressTask.Task() {
				@Override
				public void run(ProgressTask task) {
					adapter.del_shaders_cache(position);
					task.task_handler.sendEmptyMessage(ProgressTask.TASK_DONE);
					progress_task=null;
				}
			});
		}
		return super.onContextItemSelected(item);
	}

	@Override
	protected void onDestroy() {
		if(progress_task!=null){
			progress_task.force_close();
			progress_task=null;
		}
		super.onDestroy();
	}



	void show_verify_dialog(int res_id, DialogInterface.OnClickListener listener){
		new AlertDialog.Builder(this)
				.setMessage(getString(res_id)+"?")
				.setPositiveButton(android.R.string.ok, listener)
				.setNegativeButton(android.R.string.cancel, null)
				.create().show();
	}

	private void initialize(){
		//private final File APP_DIR=new File(Environment.getExternalStorageDirectory(),"aps3e");

		String[] sub_dir_paths={
			"aps3e",
			"aps3e/config",
			"aps3e/config/dev_flash",
			"aps3e/config/dev_flash2",
			"aps3e/config/dev_flash3",
			"aps3e/config/dev_bdvd",
			"aps3e/config/dev_hdd0",
			"aps3e/config/dev_hdd1",
			"aps3e/config/dev_hdd0/game",
			"aps3e/config/Icons",
				"aps3e/config/games",
			"aps3e/config/Icons/ui",
			"aps3e/logs",
				"aps3e/font",
		};

		for(String sp:sub_dir_paths){
			File dir=new File(Application.get_app_data_dir().getParent(),sp);
			if(!dir.exists()){
				dir.mkdir();
			}
		}
		
		File icons_ui_output_dir=new File(Application.get_app_data_dir().getParent(),"aps3e/config/Icons/ui");
		Application.extractAssetsDir(this,"Icons/ui",icons_ui_output_dir);

		File xxxx_txt=new File(Application.get_app_data_dir(),"xxxx.txt");
		if(!xxxx_txt.exists()) {
            FileOutputStream fos= null;
            try {
                fos = new FileOutputStream(xxxx_txt);
				fos.write(Emulator.get.generate_config_xml().getBytes());
				fos.write("\n\n".getBytes());
				fos.write(Emulator.get.generate_strings_xml().getBytes());
				fos.write("\n\n".getBytes());
				fos.write(Emulator.get.generate_java_string_arr().getBytes());
            } catch (Exception e) {

            } finally {
				try {
					if (fos != null) fos.close();
				} catch (IOException e) {
				}
			}
		}

		File config_yml_output_dir=new File(Application.get_app_data_dir().getParent(),"aps3e/config");
		if(!new File(config_yml_output_dir,"config.yml").exists()) {
			Application.extractAssetsDir(this, "config", config_yml_output_dir);
		}
		
		/*File nomedia=new File(Environment.getExternalStorageDirectory(),"aps3e/.nomedia");
		try{
			if(!nomedia.exists())
				nomedia.createNewFile();
		}
		catch (IOException e)
		{}*/

        /*if(!APP_DIR.exists())
		 APP_DIR.mkdirs();*/
    }

    private void refresh_game_list(){
		if(!get_hdd0_game_dir().exists()){
			get_hdd0_game_dir().mkdirs();
		}
		if(!get_disc_game_dir().exists()){
			get_disc_game_dir().mkdirs();
		}
		(progress_task=new ProgressTask( this)
				.set_progress_message(getString( R.string.game_list_loading))
				.set_done_task(new ProgressTask.UI_Task() {
					@Override
					public void run() {
						show_game_list();
						progress_task=null;
					}
				}))
				.call( new ProgressTask.Task() {
					@Override
					public void run(ProgressTask task) {

						File json_file=get_game_list_file();
						if(json_file.exists())
							json_file.delete();
						GameMetaInfoAdapter.save_game_list_to_json_file(json_file,GameMetaInfoAdapter.refresh_game_list(MainActivity.this));
							task.task_handler.sendEmptyMessage(ProgressTask.TASK_DONE);
					}
				});
    }

	private void show_game_list(){
		if(!get_hdd0_game_dir().exists()){
			get_hdd0_game_dir().mkdirs();
		}
		if(!get_disc_game_dir().exists()){
			get_disc_game_dir().mkdirs();
		}

        try {
            ArrayList<Emulator.MetaInfo> metas=GameMetaInfoAdapter.load_game_list_from_json_file(get_game_list_file());
			adapter=new GameMetaInfoAdapter(this, metas);
			((ListView)findViewById(R.id.game_list)).setAdapter(adapter);
        } catch (Exception e) {
			Log.w("show_game_list",e.getMessage());
			refresh_game_list();
        }

	}

    private static class GameMetaInfoAdapter extends BaseAdapter {
		
		private static final FileFilter dir_filter_=new FileFilter(){
            
            @Override
            public boolean accept(File f){
				if(f.isFile()) return false;
				for(String entry:new String[]{"PARAM.SFO","PS3_GAME/PARAM.SFO"}){
					File param=new File(f,entry);
					if(param.exists()){
						return true;
					}
				}
				return false;

            }         
        };
		
		static byte[] read_game_icon_from_dir(File game_dir) {
			File icon=new File(game_dir,"ICON0.PNG");
			if (!icon.exists()) {
				icon = new File(game_dir, "PS3_GAME/ICON0.PNG");
			}
			if (!icon.exists()) {
				return null;
			}
			try {
				FileInputStream fis = new FileInputStream(icon);
				byte[] buffer = new byte[(int) icon.length()];
				fis.read(buffer);
				return buffer;
			} catch (IOException e) {
				e.printStackTrace();
				return null;
			}
		}
		
		static void deleteDirectory(File directory) {
			if (directory.isDirectory()) {
				File[] files = directory.listFiles();
				if (files!= null) {
					for (File file : files) {
						if (file.isDirectory()) {
							deleteDirectory(file);
						} else {
							file.delete();
						}
					}
				}
			}
			directory.delete();
		}
		ArrayList<Emulator.MetaInfo> metas;
		private final MainActivity context_;
		private GameMetaInfoAdapter(MainActivity context,ArrayList<Emulator.MetaInfo> metas){
			context_=context;
			this.metas=metas;
		}
		static ArrayList<Emulator.MetaInfo> load_game_list_from_json_file(File json) throws JSONException, IOException {
			FileInputStream fis=new FileInputStream(json);
			ByteArrayOutputStream baos=new ByteArrayOutputStream();
			byte[] buffer=new byte[16384];
			int n;
			while ((n=fis.read(buffer))!=-1){
				baos.write(buffer,0,n);
			}
			fis.close();
			String json_str=baos.toString();
			return load_game_list_from_json(json_str);
		}

			static ArrayList<Emulator.MetaInfo> load_game_list_from_json(String json_str) throws JSONException {
			ArrayList<Emulator.MetaInfo> metas=new ArrayList<Emulator.MetaInfo>();
			JSONArray game_list=new JSONArray(json_str);
			for(int i=0;i<game_list.length();i++){
				JSONObject game_info=game_list.getJSONObject(i);
				Emulator.MetaInfo meta=Emulator.MetaInfo.from_json(game_info);
				metas.add(meta);
			}
			return metas;
		}

		static void save_game_list_to_json_file(File json,ArrayList<Emulator.MetaInfo> metas){
			try {
				FileOutputStream fos=new FileOutputStream(json);
				fos.write(save_game_list_to_json(metas).getBytes());
				fos.close();
			} catch (IOException | JSONException e) {
				e.printStackTrace();
			}
		}

		static String save_game_list_to_json(ArrayList<Emulator.MetaInfo> metas) throws JSONException {
			JSONArray game_list=new JSONArray();
			for(Emulator.MetaInfo meta:metas){
				game_list.put(Emulator.MetaInfo.to_json(meta));
			}
			return game_list.toString();
		}

		static ArrayList<Emulator.MetaInfo> refresh_game_list(MainActivity context){
			return gen_metas(context,get_game_dir_list(),get_game_iso_list(context));
		}

		private static ArrayList<File> get_game_dir_list(){
			ArrayList<File> game_dir_list=new ArrayList<File>();
			File[] game_dirs;
			game_dirs=get_hdd0_game_dir().listFiles(dir_filter_);
			if(game_dirs!=null)
				for(File f:game_dirs)
					game_dir_list.add(f);

			game_dirs=get_disc_game_dir().listFiles(dir_filter_);
			if(game_dirs!=null)
				for(File f:game_dirs)
					game_dir_list.add(f);

			return game_dir_list;
        }

		private static ArrayList<DocumentFile> get_game_iso_list(MainActivity context_){
			ArrayList<DocumentFile> iso_list=new ArrayList<DocumentFile>();
			Uri uri=context_.load_pref_iso_dir();
			if(uri==null)
				return iso_list;
			DocumentFile iso_dir=DocumentFile.fromTreeUri(context_, uri);
			if(iso_dir==null||!iso_dir.exists())
				return iso_list;
			DocumentFile [] iso_files=iso_dir.listFiles();
			if(iso_files==null)
				return iso_list;
			for(DocumentFile f:iso_files){
				if(f.isFile()&&f.getName().endsWith(".iso"))
					iso_list.add(f);
			}
			return iso_list;
		}
		
		private static ArrayList<Emulator.MetaInfo> gen_metas(MainActivity context_,ArrayList<File> dirs,ArrayList<DocumentFile> isos){
			ArrayList<Emulator.MetaInfo> metas=new ArrayList<Emulator.MetaInfo>();
			for(File dir:dirs){
				Emulator.MetaInfo  meta=Emulator.get.meta_info_from_dir(dir.getAbsolutePath());
				if(meta.category.equals("GD")) continue;
				meta.icon=read_game_icon_from_dir(dir);
				metas.add(meta);
			}

			for(DocumentFile iso:isos){
                try {
                    ParcelFileDescriptor pfd_=context_.getContentResolver().openFileDescriptor(iso.getUri(),"r");
					int pfd=pfd_.detachFd();
					pfd_.close();
					Emulator.MetaInfo meta=Emulator.get.meta_info_from_iso(pfd,iso.getUri().toString());
					if(meta!=null)
					metas.add(meta);
                } catch (Exception e) {

                }
			}

			return metas;
		}
		
		public Emulator.MetaInfo getMetaInfo(int pos){
            return metas.get(pos);
        }

		public boolean is_disc_game(int pos){
			Emulator.MetaInfo info=metas.get(pos);
			if(info.iso_uri!=null)
				return true;
			if(info.eboot_path.startsWith(get_disc_game_dir().getAbsolutePath()))
				return true;
			return false;
		}

		void _del_trophy_data(String serial){
			File[] trophy_dirs=get_game_trophy_dir().listFiles();
			if(trophy_dirs!=null){
				for(File f:trophy_dirs){
					if(f.getName().startsWith(serial)){
						deleteDirectory(f);
					}
				}
			}
		}
		void _del_game_data(String serial){
			File[] data_dirs=get_game_data_dir().listFiles();
			if(data_dirs!=null){
				for(File f:data_dirs){
					if(f.getName().startsWith(serial)){
						deleteDirectory(f);
					}
				}
			}
		}

		public void _del_hdd0_data(String serial){
			File[] game_dirs=get_hdd0_game_dir().listFiles();
			if(game_dirs!=null){
				for(File f:game_dirs){
					if(f.getName().startsWith(serial)){
						deleteDirectory(f);
					}
				}
			}
		}
		public void del_hdd0_game_and_data(int pos){
			Emulator.MetaInfo info=metas.get(pos);
			_del_trophy_data(info.serial);
			_del_game_data(info.serial);

			File game_dir=new File(get_hdd0_game_dir(),info.serial);
			if(game_dir.exists())
				deleteDirectory(game_dir);
		}

		public void del_hdd0_install_data(int pos){
			Emulator.MetaInfo info=metas.get(pos);
			_del_hdd0_data(info.serial);
		}

		public void del_game_data(int pos){
			Emulator.MetaInfo info=metas.get(pos);
			_del_trophy_data(info.serial);
			_del_game_data(info.serial);
		}

		public  void del_shaders_cache(int pos){
			Emulator.MetaInfo info=metas.get(pos);
			File shader_cache_dir=get_shader_cache_dir(info.serial);
			if(shader_cache_dir!=null&& shader_cache_dir.exists())
				deleteDirectory(shader_cache_dir);
		}

        @Override
        public int getCount(){
            return metas.size();
        }

        @Override
        public Object getItem(int p1){
            return null;
        }

        @Override
        public long getItemId(int p1){
            return p1;
        }

        private LayoutInflater getLayoutInflater(){
            return (LayoutInflater)context_.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

		Bitmap to_gray_bmp(Bitmap src){
			int w=src.getWidth();
			int h=src.getHeight();
			Bitmap bmp=Bitmap.createBitmap(w,h,src.getConfig());
			ColorMatrix cm=new ColorMatrix();
			cm.setSaturation(0);
			Canvas canvas=new Canvas(bmp);
			Paint paint=new Paint();
			paint.setColorFilter(new ColorMatrixColorFilter(cm));
			canvas.drawBitmap(src,0,0,paint);
			return bmp;
        }

        @Override
        public View getView(int pos,View curView,ViewGroup p3){

            if(curView==null){
                curView=getLayoutInflater().inflate(R.layout.game_item,null);
            }
			
			Emulator.MetaInfo mi=metas.get(pos);

            ImageView icon=(ImageView)curView.findViewById(R.id.game_icon);
			if(mi.icon==null)
				icon.setImageResource(R.drawable.unknown);
			else {
				Bitmap icon_bmp=BitmapFactory.decodeByteArray(mi.icon,0,mi.icon.length);
				if(!mi.decrypt)
					icon_bmp=to_gray_bmp(icon_bmp);
				icon.setImageBitmap(icon_bmp);
			}
			
			TextView name=(TextView)curView.findViewById(R.id.game_name);
			name.setText(mi.name);
			
            TextView version=(TextView)curView.findViewById(R.id.game_version);
			version.setText(mi.version);
			
			TextView serial=(TextView)curView.findViewById(R.id.game_serial);
			serial.setText(mi.serial);
			
			TextView category=(TextView)curView.findViewById(R.id.game_category);
			category.setText(mi.category);

            return curView;
        } 
    }//!FileAdapter
}
