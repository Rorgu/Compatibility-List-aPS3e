// SPDX-License-Identifier: WTFPL
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

    public interface UI_Task {
        void run();
    }

        public static final int TASK_FAILED=0xAA000000;
        public static final int TASK_DONE=0xAA000001;

        private Dialog progress_dialog;
    Context context;
    private Thread task_thread;
    String progress_message;

    UI_Task failed_task;
    UI_Task done_task;

    public Handler task_handler=new Handler() {
        @Override
        public void handleMessage(Message msg) {
            if(progress_dialog!=null&&progress_dialog.isShowing()){
            progress_dialog.hide();
            progress_dialog.dismiss();
            progress_dialog = null;}

            task_thread = null;

            try {
                if (msg.what == TASK_FAILED){
                    if(failed_task!=null)
                        failed_task.run();
                }
                else if (msg.what == TASK_DONE){
                    if(done_task!=null)
                    done_task.run();
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
        }

        public ProgressTask set_progress_message(CharSequence message){
            progress_message=message.toString();
            return this;
        }
        public ProgressTask set_failed_task(UI_Task  task){
            failed_task=task;
            return this;
        }

        public ProgressTask set_done_task(UI_Task  task){
            done_task=task;
            return this;
        }

        void force_close(){
            if(task_thread!=null){
                task_thread.interrupt();
                task_thread=null;
            }
            if(progress_dialog!=null&&progress_dialog.isShowing()){
                progress_dialog.hide();
                progress_dialog.dismiss();
                progress_dialog=null;
            }
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
