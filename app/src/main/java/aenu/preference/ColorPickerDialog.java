// SPDX-License-Identifier: WTFPL
package aenu.preference;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.os.Bundle;
import android.util.AttributeSet;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.preference.DialogPreference;
import androidx.preference.ListPreferenceDialogFragmentCompat;
import androidx.preference.PreferenceDialogFragmentCompat;

import aenu.aps3e.R;

public class ColorPickerDialog extends DialogPreference {

    public interface OnColorChangedListener{
        public void onColorChanged(int color);
    }

    public static class ColorPickerView extends android.view.View{

        public ColorPickerView(Context context) {
            super(context);
        }

        public ColorPickerView(Context context, AttributeSet attrs) {
            super(context, attrs);
        }

        public ColorPickerView(Context context, AttributeSet attrs, int defStyleAttr) {
            super(context, attrs, defStyleAttr);
        }

        Bitmap generate_color_picker_bitmap(){
            int[] colors=new int[256*256];
            int i=0;
            for(int r=1;r<=5;r++){
                for(int g=1;g<=5;g++){
                    for(int b=1;b<=5;b++){
                        for(int a=0;a<=1;a++){
                            colors[i++]= Color.argb(a*255,r*51,g*51,b*51);
                        }
                    }
                }
            }
            Bitmap bmp=Bitmap.createBitmap(colors,0,256,256,256, Bitmap.Config.ARGB_8888);
            return bmp;
        }

        @Override
        protected void onDraw(Canvas canvas) {
            canvas.drawColor(Color.WHITE);
            canvas.drawBitmap(generate_color_picker_bitmap(),0,0,null);

        }
    }

    public static class ColorPickerPreferenceFragmentCompat extends PreferenceDialogFragmentCompat {

        @NonNull
        public static ColorPickerPreferenceFragmentCompat newInstance(String key) {
            final ColorPickerPreferenceFragmentCompat fragment =
                    new ColorPickerPreferenceFragmentCompat();
            final Bundle b = new Bundle(1);
            b.putString(ARG_KEY, key);
            fragment.setArguments(b);
            return fragment;
        }
        @Override
        public void onDialogClosed(boolean positiveResult) {

        }
    }

    static final String NS="http://schemas.android.com/apk/aenu.pref";

    public ColorPickerDialog(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
    }
    public ColorPickerDialog(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        setDialogLayoutResource(R.layout.preference_color_picker);
    }
    public ColorPickerDialog(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        setDialogLayoutResource(R.layout.preference_color_picker);
    }
}
