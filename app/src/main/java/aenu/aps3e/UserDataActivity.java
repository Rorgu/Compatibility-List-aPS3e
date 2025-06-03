package aenu.aps3e;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.provider.DocumentsContract;
import android.provider.DocumentsProvider;

import androidx.appcompat.app.AppCompatActivity;

public class UserDataActivity extends AppCompatActivity {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //open_file_manager();
    }

    static Intent get_file_manager_intent(String action)
    {
        Intent it=new Intent(action);
        it.addCategory(Intent.CATEGORY_DEFAULT);
        it.setData(DocumentsContract.buildRootUri("$packageName.user", "root"));
        it.addFlags(Intent.FLAG_GRANT_PREFIX_URI_PERMISSION|Intent.FLAG_GRANT_WRITE_URI_PERMISSION|Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION);
        return it;
    }

    static Intent get_file_manager_intent_2(String pkg_name)
    {
        Intent it=new Intent(Intent.ACTION_VIEW);
        it.setClassName(pkg_name, "com.android.documentsui.files.FilesActivity");
        it.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        return it;
    }
    static void open_file_manager(Activity activity)
    {
        /*try{
            startActivity(get_file_manager_intent(Intent.ACTION_VIEW));
            return;
        }
        catch(Exception e){
        }

        try{
            startActivity(get_file_manager_intent("android.provider.action.BROWSE"));
            return;
        }
        catch(Exception e){
        }*/

        try{
            activity.startActivity(get_file_manager_intent_2("com.android.documentsui"));
            return;
        }
        catch(Exception e){
        }
        try{
            activity.startActivity(get_file_manager_intent_2("com.google.android.documentsui"));
            return;
        }
        catch(Exception e){
        }
    }
}
