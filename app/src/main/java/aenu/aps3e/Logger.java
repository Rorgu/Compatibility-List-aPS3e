// SPDX-License-Identifier: WTFPL

package aenu.aps3e;
import java.io.*;
import java.util.Date;

import android.os.*;
import android.icu.text.*;
import android.content.*;

public class Logger
{ 
	public static void proc_info(){
		File proc_dir=new File("/proc/self");
		File[] files=proc_dir.listFiles();
	}
	
	public static void start_record(final Context ctx){
		new Thread(){
			public void run(){
				File log_dir=Application.get_app_log_dir();
				
				try
				{
					SimpleDateFormat date_fmt=new SimpleDateFormat("yyMMdd_HHmmss");
					Date cur_date=new Date(System.currentTimeMillis());
					File log_file=new File(log_dir,"aps3e_java_"+date_fmt.format(cur_date)+"_"+android.os.Process.myPid()+".txt");
					String cmd=String.format("/system/bin/logcat -f %s *:I",log_file.getAbsolutePath());
					Runtime.getRuntime().exec(cmd).getInputStream();
				}
				catch (IOException e)
				{
					try{
						File log_file=new File(log_dir,"aps3e_fail.txt");
					if(!log_file.exists())log_file.createNewFile();
					FileOutputStream fout=new FileOutputStream(log_file,true);
					PrintStream print=new PrintStream(fout);
					e.printStackTrace(print);
					print.close();
					fout.close();
					}catch(Exception _e){}
				}
			}
		}.start();
	}
}
