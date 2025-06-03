package aenu.aps3e;

import android.app.*;
import android.net.Uri;
import android.os.*;
import android.view.*;
import android.content.*;
import android.widget.*;
import android.preference.*;
import android.util.*;
import org.vita3k.emulator.overlay.InputOverlay.ControlId;
import android.content.res.*;

//import org.libsdl.app.*;

public class EmulatorActivity extends Activity implements View.OnGenericMotionListener,SurfaceHolder.Callback
{
    { System.loadLibrary("e"); }

    private SparseIntArray keysMap = new SparseIntArray();
    private GameFrameView gv;

	boolean started=false;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

		VirtualPadEdit.enable_fullscreen(getWindow());

        setContentView(R.layout.emulator_view);
        gv=(GameFrameView)findViewById(R.id.emulator_view);

        gv.setFocusable(true);
        gv.setFocusableInTouchMode(true);
        gv.requestFocus();
		
		gv.setOnGenericMotionListener(this);

		gv.getHolder().addCallback(this);
		
        load_key_map();
		Emulator.MetaInfo meta_info = (Emulator.MetaInfo) getIntent().getSerializableExtra("meta_info");
		if(meta_info.eboot_path!=null&&meta_info.iso_uri==null)
		Emulator.get.setup_game_info(meta_info);
		else if(meta_info.eboot_path==null&&meta_info.iso_uri!=null){

			try {
				ParcelFileDescriptor pfd_= getContentResolver().openFileDescriptor(Uri.parse(meta_info.iso_uri), "r");
				meta_info.iso_fd=pfd_.detachFd();
				pfd_.close();

				Emulator.get.setup_game_info(meta_info);
			} catch (Exception e) {
				Toast.makeText(this, e.getMessage(), Toast.LENGTH_SHORT).show();
				return;
			}
		}
		else {
			throw new RuntimeException("Invalid meta info");
		}
	}

	@Override
	protected void onStart() {
		super.onStart();
	}

	@Override
	public void onBackPressed()
	{
		
		AlertDialog.Builder ab=new AlertDialog.Builder(this);
		ab.setPositiveButton(R.string.quit, new DialogInterface.OnClickListener(){

				@Override
				public void onClick(DialogInterface p1, int p2)
				{
					try{
                        //if(Emulator.get.is_running())
                        //Emulator.get.pause();
						Emulator.get.quit();
				}catch(Exception e){}
				finally{
					p1.cancel();
					finish();
				}
		}
				
			
		});
        
        /*ab.setNegativeButton("TE", new DialogInterface.OnClickListener(){

                @Override
                public void onClick(DialogInterface p1, int p2)
                {
                    if(Emulator.get.is_running())
                         Emulator.get.pause();
                     else if(Emulator.get.is_paused())
                         Emulator.get.resume();
                }
                
            
        });*/
        //if(Emulator.get.is_running())
		//Emulator.get.pause();
		ab.create().show();
	}
	
	@Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        int gameKey = keysMap.get(keyCode, 0);
		if (gameKey == 0) return super.onKeyDown(keyCode, event);
		if (event.getRepeatCount() == 0){
			Emulator.get.key_event(gameKey, true);
            return true;
		}
		return super.onKeyDown(keyCode, event);
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        int gameKey = keysMap.get(keyCode, 0);
        if (gameKey != 0) {
            Emulator.get.key_event(gameKey, false);
			return true;
        }
        return super.onKeyUp(keyCode, event);
    }

	boolean handle_dpad(InputEvent event) {
		
		boolean pressed=false;
		if (event instanceof MotionEvent) {

            // Use the hat axis value to find the D-pad direction
            MotionEvent motionEvent = (MotionEvent) event;
            float xaxis = motionEvent.getAxisValue(MotionEvent.AXIS_HAT_X);
            float yaxis = motionEvent.getAxisValue(MotionEvent.AXIS_HAT_Y);

            // Check if the AXIS_HAT_X value is -1 or 1, and set the D-pad
            // LEFT and RIGHT direction accordingly.
            if (Float.compare(xaxis, -1.0f) == 0) {
                Emulator.get.key_event(ControlId.l, true);
				pressed=true;
            } else if (Float.compare(xaxis, 1.0f) == 0) {
                Emulator.get.key_event(ControlId.r, true);
				pressed=true;
            }
            // Check if the AXIS_HAT_Y value is -1 or 1, and set the D-pad
            // UP and DOWN direction accordingly.
            if (Float.compare(yaxis, -1.0f) == 0) {
                Emulator.get.key_event(ControlId.u, true);
				pressed=true;
            } else if (Float.compare(yaxis, 1.0f) == 0) {
                Emulator.get.key_event(ControlId.d, true);
				pressed=true;
            }
        }
		else if (event instanceof KeyEvent) {

			// Use the key code to find the D-pad direction.
            KeyEvent keyEvent = (KeyEvent) event;
            if (keyEvent.getKeyCode() == KeyEvent.KEYCODE_DPAD_LEFT) {
                Emulator.get.key_event(ControlId.l, true);
				pressed=true;
				
            } else if (keyEvent.getKeyCode() == KeyEvent.KEYCODE_DPAD_RIGHT) {
                Emulator.get.key_event(ControlId.r, true);
				pressed=true;
				
            } else if (keyEvent.getKeyCode() == KeyEvent.KEYCODE_DPAD_UP) {
                Emulator.get.key_event(ControlId.u, true);
				pressed=true;
				
            } else if (keyEvent.getKeyCode() == KeyEvent.KEYCODE_DPAD_DOWN) {
                Emulator.get.key_event(ControlId.d, true);
				pressed=true;
				
            }
		}
		
		if(pressed) return true;
		Emulator.get.key_event(ControlId.l, false);
		Emulator.get.key_event(ControlId.u, false);
		Emulator.get.key_event(ControlId.r, false);
		Emulator.get.key_event(ControlId.d, false);
		return false;
	}


    private static boolean isDpadDevice(MotionEvent event) {
        // Check that input comes from a device with directional pads.
        if ((event.getSource() & InputDevice.SOURCE_DPAD)
			!= InputDevice.SOURCE_DPAD) {
            return true;
        } else {
            return false;
        }
    }
	
	@Override
    public boolean onGenericMotion(View v, MotionEvent event) {
		
		if(isDpadDevice(event)&& handle_dpad(event)) return true;
		
		if ((event.getSource() & InputDevice.SOURCE_JOYSTICK) == InputDevice.SOURCE_JOYSTICK&&
			event.getAction() == MotionEvent.ACTION_MOVE) {
			float laxisX = event.getAxisValue(MotionEvent.AXIS_X);
			float laxisY = event.getAxisValue(MotionEvent.AXIS_Y);
			float raxisX = event.getAxisValue(MotionEvent.AXIS_Z);
			float raxisY = event.getAxisValue(MotionEvent.AXIS_RZ);

			for(int i=ControlId.lsl;i<ControlId.rsd;i++){
				Emulator.get.key_event(i,false);
			}

			{

				//左摇杆


				if(Math.abs(laxisX)>0.37){
					if(laxisX<0){
						Emulator.get.key_event(ControlId.lsr,false);
						Emulator.get.key_event(ControlId.lsl,true,(int)(Math.abs(laxisX)*255.0));
					}
					else{
						Emulator.get.key_event(ControlId.lsl,false);
						Emulator.get.key_event(ControlId.lsr,true,(int)(Math.abs(laxisX)*255.0));
					}
				}

				if(Math.abs(laxisY)>0.37){
					if(laxisY<0){
						Emulator.get.key_event(ControlId.lsd,false);
						Emulator.get.key_event(ControlId.lsu,true,(int)(Math.abs(laxisY)*255.0));
					}else{
						Emulator.get.key_event(ControlId.lsu,false);
						Emulator.get.key_event(ControlId.lsd,true,(int)(Math.abs(laxisY)*255.0));
					}
				}
			}
			{
				if(Math.abs(raxisX)>0.37){
					if(raxisX<0){
						Emulator.get.key_event(ControlId.rsr,false);
						Emulator.get.key_event(ControlId.rsl,true,(int)(Math.abs(raxisX)*255.0));
					}else{
						Emulator.get.key_event(ControlId.rsl,false);
						Emulator.get.key_event(ControlId.rsr,true,(int)(Math.abs(raxisX)*255.0));
					}
				}

				if(Math.abs(raxisY)>0.37){
					if(raxisY<0){
						Emulator.get.key_event(ControlId.rsd,false);
						Emulator.get.key_event(ControlId.rsu,true,(int)(Math.abs(raxisY)*255.0));
					}else{
						Emulator.get.key_event(ControlId.rsu,false);
						Emulator.get.key_event(ControlId.rsd,true,(int)(Math.abs(raxisY)*255.0));
					}

				}
			}
			return true;
		}
		
		return super.onGenericMotionEvent(event);
	}
	
	
	
	void load_key_map() {
        final SharedPreferences sPrefs = PreferenceManager.getDefaultSharedPreferences(this);
        keysMap.clear();
        for (int i = 0; i < KeyMapConfig.KEY_NAMEIDS.length; i++) {
            String keyName = Integer.toString(KeyMapConfig.KEY_NAMEIDS[i]);
            int keyCode = sPrefs.getInt(keyName, KeyMapConfig.DEFAULT_KEYMAPPERS[i]);
            keysMap.put(keyCode, KeyMapConfig.KEY_VALUES[i]);
        }
    }

	@Override
	protected void onPause()
	{
		super.onPause();

		if(started)
			if(Emulator.get.is_running())
				Emulator.get.pause();
	}

	@Override
	protected void onResume()
	{
		super.onResume();

		if(started&&gv.getHolder().getSurface().isValid()&&Emulator.get.is_paused()){
				Emulator.get.resume();
		}

	}

	@Override
	protected void onStop() {
		super.onStop();
	}

	@Override
	protected void onDestroy()
	{
		super.onDestroy();
		System.exit(0);
	}

    @Override
    public void onConfigurationChanged(Configuration newConfig)
    {
        // TODO: Implement this method
        super.onConfigurationChanged(newConfig);
    }

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		//synchronized (EmulatorActivity.class)
        {

			if (!started) {
				started = true;
				try {
					Emulator.get.setup_surface(holder.getSurface());
				} 
                finally {
					try {
						Emulator.get.boot();
					} catch (Emulator.BootException e) {
						throw new RuntimeException(e);
					}
				}

			} else {
				
					Emulator.get.setup_surface(holder.getSurface());
				
				if (Emulator.get.is_paused())
					Emulator.get.resume();
			}
		}
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
		if (!started) return;
		if(width==0||height==0) return;
		//if (Emulator.get.is_running()) Emulator.get.pause();
		//while (!Emulator.get.is_paused());
		Emulator.get.change_surface(width,height);
		//Emulator.get.resume();
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		//synchronized (EmulatorActivity.class){
		if(!started)
			return;
        Emulator.get.setup_surface(null);
    //}
	}
}
