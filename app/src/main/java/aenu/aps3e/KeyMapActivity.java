// SPDX-License-Identifier: WTFPL

package aenu.aps3e;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.PreferenceActivity;
import android.preference.PreferenceManager;
import android.preference.PreferenceScreen;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.Toast;

import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.ArrayList;
import java.util.List;
import org.json.JSONObject;
import android.widget.Adapter;
import android.app.*;
import android.widget.*;
import java.util.*;
import android.content.*;
import android.icu.text.*;
import android.view.*;

public class KeyMapActivity extends Activity {
    
	SharedPreferences sp;
	ListView lv;
    @Override
    public void onCreate(Bundle bundle) {
        super.onCreate(bundle);
		sp=getSharedPreferences();
        lv=new ListView(this);
		lv.setDividerHeight(32);
		setContentView(lv);
		update_config();
		lv.setOnItemClickListener(click_l);
		refresh_view();
    }
	
	void refresh_view(){
		lv.setAdapter(new KeyListAdapter(this,KeyMapConfig.KEY_NAMEIDS,get_all_key_mapper_values()));
	}
	
	void update_config(){
		final SharedPreferences sPrefs = PreferenceManager.getDefaultSharedPreferences(KeyMapActivity.this);
		SharedPreferences.Editor sPrefsEditor = sPrefs.edit();

		for(int i=0;i<KeyMapConfig.KEY_NAMEIDS.length;i++){
			String key_n=Integer.toString(KeyMapConfig.KEY_NAMEIDS[i]);
			int default_v=KeyMapConfig.DEFAULT_KEYMAPPERS[i];
			int key_v=sPrefs.getInt(key_n,default_v);
			sPrefsEditor.putInt(key_n,key_v);
		}
		
		sPrefsEditor.commit();
	}
	
	int[] get_all_key_mapper_values(){
		final SharedPreferences sPrefs = PreferenceManager.getDefaultSharedPreferences(KeyMapActivity.this);
		
		int[] key_values=new int[KeyMapConfig.KEY_NAMEIDS.length];
		for(int i=0;i<KeyMapConfig.KEY_NAMEIDS.length;i++){
			String key_n=Integer.toString(KeyMapConfig.KEY_NAMEIDS[i]);
			key_values[i]=sPrefs.getInt(key_n,0);
		}
		return key_values;
	}
	
	private final AdapterView.OnItemClickListener click_l=new AdapterView.OnItemClickListener(){
		@Override
		public void onItemClick(final AdapterView<?> l, View v, final int position,long id)
		{
			AlertDialog.Builder builder = new AlertDialog.Builder(KeyMapActivity.this);
            builder.setMessage(R.string.press_a_key);
            builder.setNegativeButton(R.string.clear, new DialogInterface.OnClickListener(){
					@Override
					public void onClick(DialogInterface p1, int p2)
					{
						final SharedPreferences sPrefs = PreferenceManager.getDefaultSharedPreferences(KeyMapActivity.this);
						SharedPreferences.Editor sPrefsEditor = sPrefs.edit();
						sPrefsEditor.putInt((String)l.getItemAtPosition(position),0);
						sPrefsEditor.commit();
						refresh_view();
					}
				});
			builder.setOnKeyListener(new DialogInterface.OnKeyListener(){
					@Override
					public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event)
					{
						if (event.getAction() == KeyEvent.ACTION_DOWN) {
							final SharedPreferences sPrefs = PreferenceManager.getDefaultSharedPreferences(KeyMapActivity.this);
							SharedPreferences.Editor sPrefsEditor = sPrefs.edit();
							sPrefsEditor.putInt((String)l.getItemAtPosition(position),keyCode);
							sPrefsEditor.commit();
							dialog.dismiss();
							refresh_view();
							return true;
						}
						return false;
					}
			});
            AlertDialog dialog = builder.create();
            dialog.show();  
		}
	};

    private SharedPreferences getSharedPreferences() {
        return PreferenceManager.getDefaultSharedPreferences(this);
    }
	
	private static class KeyListAdapter extends BaseAdapter {

        private int[] keyNameIdList_;
		private int[] valueList_;
        private Context context_; 

        private KeyListAdapter(Context context,int[] keyList,int[] valueList){
            context_=context;
			this.keyNameIdList_=keyList;
			this.valueList_=valueList;
		}

        public String getKey(int pos){
            return Integer.toString(keyNameIdList_[pos]);
        }

		public String getKeyName(int pos){
            return context_.getString(keyNameIdList_[pos]);
        }

        @Override
        public int getCount(){
            return keyNameIdList_.length;
        }

        @Override
        public Object getItem(int p1){
            return getKey(p1);
        }

        @Override
        public long getItemId(int p1){
            return p1;
        }

        @Override
        public View getView(int pos,View curView,ViewGroup p3){

            
            if(curView==null){
                curView=new TextView(context_);
            }
			
			TextView text=(TextView)curView;

            text.setText(getKeyName(pos)+":    "+valueList_[pos]);

            return curView;
        } 
    }//!FileAdapter
}
