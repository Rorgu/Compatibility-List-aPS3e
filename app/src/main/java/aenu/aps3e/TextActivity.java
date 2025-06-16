// SPDX-License-Identifier: WTFPL
package aenu.aps3e;

import android.app.Activity;
import android.os.Bundle;
import android.widget.ScrollView;
import android.widget.TextView;

public class TextActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        String txt=getIntent().getStringExtra("text");
        TextView tv = new TextView(this);
        tv.setText(txt);
        tv.setTextIsSelectable(true);
        tv.setLongClickable(true);

        ScrollView sv=new ScrollView(this);
        sv.addView(tv);
        setContentView(sv);
    }

}
