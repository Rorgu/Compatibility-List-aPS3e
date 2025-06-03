package aenu.aps3e;

import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Handler;
import android.os.Message;
import android.view.KeyEvent;
import android.widget.Toast;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;

public class ProgressTask {

    public interface Task {
        void run(ProgressTask task);
    }

        public static final int TASK_FAILED=0xAA000000;
        public static final int TASK_DONE=0xAA000001;

        private Dialog progress_dialog;
    Context context;
    private Thread task_thread;
    String progress_message;

    String failed_message;
    String  done_message;
    public Handler task_handler=new Handler() {
        @Override
        public void handleMessage(Message msg) {
            progress_dialog.hide();
            progress_dialog.dismiss();
            progress_dialog = null;

            task_thread = null;

            try {
                if (msg.what == TASK_FAILED)
                    Toast.makeText(context, failed_message, Toast.LENGTH_LONG).show();
                else if (msg.what == TASK_DONE){
                    if(done_message!=null)
                        Toast.makeText(context, done_message, Toast.LENGTH_SHORT).show();
                }
                else
                    android.util.Log.w("aps3e_java", "unknown message -- " + msg.what);
            } catch (Exception e) {
            }
        }
    };
        private final Dialog create_progress_dialog(Context context){
            ProgressDialog d=new ProgressDialog(context);
            d.setMessage(progress_message);
            d.setCanceledOnTouchOutside(false);
            d.setOnKeyListener(new DialogInterface.OnKeyListener(){
                @Override
                public boolean onKey(DialogInterface p1, int p2, KeyEvent p3){
                    return true;
                }
            });
            return d;
        }

        public ProgressTask(Context context){
            this.context=context;
            progress_message=context.getString(R.string.msg_processing);
            failed_message=context.getString(R.string.msg_failed);
        }

        public ProgressTask set_progress_message(CharSequence message){
            progress_message=message.toString();
            return this;
        }
        public ProgressTask set_failed_message(CharSequence message){
            failed_message=message.toString();
            return this;
        }

        public ProgressTask set_done_message(String message){
            done_message=message;
            return this;
        }

        void call(Task task){
            progress_dialog=create_progress_dialog(context);
            progress_dialog.show();
            task_thread=new Thread(new Runnable() {
                @Override
                public void run() {
                    task.run(ProgressTask.this);
                }
            });
            task_thread.start();
        }

}
