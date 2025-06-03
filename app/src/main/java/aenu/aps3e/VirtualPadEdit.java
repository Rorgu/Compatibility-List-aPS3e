package aenu.aps3e;
import android.annotation.TargetApi;
import android.app.*;
import android.os.*;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
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
		float scale=iv.getScale();

		((TextView)d.findViewById(R.id.virtual_pad_scale_hint)).setText(scale_text+scale);
		((SeekBar)d.findViewById(R.id.virtual_pad_scale)).setProgress((int)(scale*100));
		((SeekBar)d.findViewById(R.id.virtual_pad_scale)).setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener(){

			float scale;
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				scale=progress/100.f;
				((TextView)d.findViewById(R.id.virtual_pad_scale_hint)).setText(scale_text+scale);
			}

			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {

			}

			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
				iv.setScale(scale);
			}
		});

	}

	@Override
	public void onBackPressed()
	{
		create_option_menu();
	}

}
