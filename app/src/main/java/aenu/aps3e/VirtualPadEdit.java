// SPDX-License-Identifier: WTFPL
package aenu.aps3e;
import android.annotation.TargetApi;
import android.app.*;
import android.os.*;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.widget.TextView;
import android.window.OnBackInvokedDispatcher;

import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.core.view.WindowInsetsControllerCompat;

import org.vita3k.emulator.overlay.*;

public class VirtualPadEdit extends Activity
{

	public static void enable_fullscreen(Window w){
		WindowCompat.setDecorFitsSystemWindows(w,false);
		WindowInsetsControllerCompat wic=WindowCompat.getInsetsController(w,w.getDecorView());
		wic.hide(WindowInsetsCompat.Type.systemBars());
		wic.setSystemBarsBehavior(WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
		WindowManager.LayoutParams lp=w.getAttributes();
		lp.layoutInDisplayCutoutMode=WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
		w.setAttributes(lp);
	}

	InputOverlay iv=null;
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		// TODO: Implement this method
		super.onCreate(savedInstanceState);
		enable_fullscreen(getWindow());

		setContentView(iv=new InputOverlay(this,null));
		iv.setIsInEditMode(true);
		if(Build.VERSION.SDK_INT>=33){
			reg_onBackPressed();
		}
	}

	@TargetApi(33)
	void reg_onBackPressed(){
		getOnBackInvokedDispatcher().registerOnBackInvokedCallback(
				OnBackInvokedDispatcher.PRIORITY_DEFAULT,
				()->{
					create_option_menu();
				}
		);
	}

	void create_option_menu(){
		final Dialog d=new AlertDialog.Builder(this, androidx.appcompat.R.style.Theme_AppCompat_Light_Dialog_Alert)
				.setView(R.layout.pad_edit_menu)
				.create();

		d.show();

		d.findViewById(R.id.virtual_pad_reset).setOnClickListener(v->{
			iv.resetButtonPlacement();
			d.dismiss();
		});

		d.findViewById(R.id.virtual_pad_save_quit).setOnClickListener(v->{
			d.dismiss();
			finish();
		});

		final String scale_text=getString(R.string.scale_rate)+": ";
		float joystick_scale=iv.getScale(InputOverlay.ScaleType.JOYSTICK);
		float dpad_scale=iv.getScale(InputOverlay.ScaleType.DPAD);
		float abxy_scale=iv.getScale(InputOverlay.ScaleType.ABXY);
		float ss_scale=iv.getScale(InputOverlay.ScaleType.START_SELECT);
		float lr_scale=iv.getScale(InputOverlay.ScaleType.LR);
		float ps_scale=iv.getScale(InputOverlay.ScaleType.PS);

		((TextView)d.findViewById(R.id.virtual_pad_joystick_scale_hint)).setText(scale_text+joystick_scale);
		((TextView)d.findViewById(R.id.virtual_pad_dpad_scale_hint)).setText(scale_text+dpad_scale);
		((TextView)d.findViewById(R.id.virtual_pad_button_scale_hint)).setText(scale_text+abxy_scale);
		((TextView)d.findViewById(R.id.virtual_pad_ss_scale_hint)).setText(scale_text+ss_scale);
		((TextView)d.findViewById(R.id.virtual_pad_lr_scale_hint)).setText(scale_text+lr_scale);
		((TextView)d.findViewById(R.id.virtual_pad_ps_scale_hint)).setText(scale_text+ps_scale);

		((SeekBar)d.findViewById(R.id.virtual_pad_joystick_scale)).setProgress((int)(joystick_scale*100));
		((SeekBar)d.findViewById(R.id.virtual_pad_joystick_scale)).setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener(){
			float scale;
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				scale=progress/100.f;
				((TextView)d.findViewById(R.id.virtual_pad_joystick_scale_hint)).setText(scale_text+scale);
			}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {}
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {iv.setScale(InputOverlay.ScaleType.JOYSTICK,scale);}
		});

		((SeekBar)d.findViewById(R.id.virtual_pad_dpad_scale)).setProgress((int)(dpad_scale*100));
		((SeekBar)d.findViewById(R.id.virtual_pad_dpad_scale)).setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener(){
			float scale;
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				scale=progress/100.f;
				((TextView)d.findViewById(R.id.virtual_pad_dpad_scale_hint)).setText(scale_text+scale);
			}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {}
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {iv.setScale(InputOverlay.ScaleType.DPAD,scale);}
		});

		((SeekBar)d.findViewById(R.id.virtual_pad_button_scale)).setProgress((int)(abxy_scale*100));
		((SeekBar)d.findViewById(R.id.virtual_pad_button_scale)).setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener(){
			float scale;
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				scale=progress/100.f;
				((TextView)d.findViewById(R.id.virtual_pad_button_scale_hint)).setText(scale_text+scale);
			}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {}
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {iv.setScale(InputOverlay.ScaleType.ABXY,scale);}
		});

		((SeekBar)d.findViewById(R.id.virtual_pad_ss_scale)).setProgress((int)(ss_scale*100));
		((SeekBar)d.findViewById(R.id.virtual_pad_ss_scale)).setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener(){
			float scale;
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				scale=progress/100.f;
				((TextView)d.findViewById(R.id.virtual_pad_ss_scale_hint)).setText(scale_text+scale);
			}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {}
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {iv.setScale(InputOverlay.ScaleType.START_SELECT,scale);}
		});

		((SeekBar)d.findViewById(R.id.virtual_pad_lr_scale)).setProgress((int)(lr_scale*100));
		((SeekBar)d.findViewById(R.id.virtual_pad_lr_scale)).setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener(){
			float scale;
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				scale=progress/100.f;
				((TextView)d.findViewById(R.id.virtual_pad_lr_scale_hint)).setText(scale_text+scale);
			}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {}
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {iv.setScale(InputOverlay.ScaleType.LR,scale);}
		});

		((SeekBar)d.findViewById(R.id.virtual_pad_ps_scale)).setProgress((int)(ps_scale*100));
		((SeekBar)d.findViewById(R.id.virtual_pad_ps_scale)).setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener(){
			float scale;
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				scale=progress/100.f;
				((TextView)d.findViewById(R.id.virtual_pad_ps_scale_hint)).setText(scale_text+scale);
			}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {}
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {iv.setScale(InputOverlay.ScaleType.PS,scale);}
		});

		final int[] dynamic_joysticks={
			R.id.virtual_pad_dynamic_joystick_disable,
			R.id.virtual_pad_dynamic_joystick_use_left,
			R.id.virtual_pad_dynamic_joystick_use_right
		};
		//get_dynamic_joystick return -1 0 1
		((RadioGroup)d.findViewById(R.id.virtual_pad_dynamic_joystick))
				.check(dynamic_joysticks[iv.get_dynamic_joystick()+1]);
		((RadioGroup)d.findViewById(R.id.virtual_pad_dynamic_joystick)).setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
			@Override
			public void onCheckedChanged(RadioGroup group, int checkedId) {
				int dynamic_joystick;
				for(int i=0;i<dynamic_joysticks.length;i++){
					if(dynamic_joysticks[i]==checkedId){
						iv.set_dynamic_joystick(i-1);
						return;
					}
				}
			}
		});

	}

	@Override
	public void onBackPressed()
	{
		create_option_menu();
	}

}
