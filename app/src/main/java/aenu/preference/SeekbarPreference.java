// SPDX-License-Identifier: Apache-2.0
// androidx.preference

package aenu.preference;

import android.content.Context;
import android.content.DialogInterface;
import android.content.res.TypedArray;
import android.os.Bundle;
import android.os.Parcel;
import android.os.Parcelable;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.preference.DialogPreference;
import androidx.preference.ListPreference;
import androidx.preference.ListPreferenceDialogFragmentCompat;
import androidx.preference.PreferenceDialogFragmentCompat;
import androidx.preference.PreferenceViewHolder;
import androidx.preference.SeekBarPreference;

import androidx.preference.R;

public class SeekbarPreference extends DialogPreference {


    public static class SeekbarPreferenceFragmentCompat extends PreferenceDialogFragmentCompat {

        private static final String SAVE_STATE_VALUE = "SeekbarPreferenceFragmentCompat.value";
        private static final String SAVE_STATE_MIN = "SeekbarPreferenceFragmentCompat.min";
        private static final String SAVE_STATE_MAX =
                "SeekbarPreferenceFragmentCompat.max";

        int m_value;
        int m_min;
        int m_max;

        EditText mEditText;

        @NonNull
        public static SeekbarPreference.SeekbarPreferenceFragmentCompat newInstance(String key) {
            final SeekbarPreference.SeekbarPreferenceFragmentCompat fragment =
                    new SeekbarPreference.SeekbarPreferenceFragmentCompat();
            final Bundle b = new Bundle(1);
            b.putString(ARG_KEY, key);
            fragment.setArguments(b);
            return fragment;
        }

        @Override
        public void onCreate(@Nullable Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
            if (savedInstanceState == null) {
                final SeekbarPreference preference = getSeekbarPreference();
                m_value = preference.getValue();
                m_min = preference.getMin();
                m_max = preference.getMax();
            } else {
                m_value = savedInstanceState.getInt(SAVE_STATE_VALUE);
                m_min = savedInstanceState.getInt(SAVE_STATE_MIN);
                m_max = savedInstanceState.getInt(SAVE_STATE_MAX);
            }
        }

        @Override
        public void onSaveInstanceState(@NonNull Bundle outState) {
            super.onSaveInstanceState(outState);
            outState.putInt(SAVE_STATE_VALUE, m_value);
            outState.putInt(SAVE_STATE_MIN, m_min);
            outState.putInt(SAVE_STATE_MAX, m_max);
        }

        private SeekbarPreference getSeekbarPreference() {
            return (SeekbarPreference) getPreference();
        }


        @Override
        protected void onBindDialogView(@NonNull View view) {
            super.onBindDialogView(view);
            mEditText = (EditText) view.findViewById(android.R.id.edit);
            if(mEditText!=null){
                mEditText.setHint(Integer.toString(m_min)+"  ->  "+Integer.toString(m_max));
            }
        }

        @Override
        protected void onPrepareDialogBuilder(@NonNull AlertDialog.Builder builder) {
            super.onPrepareDialogBuilder(builder);

            // The typical interaction for list-based dialogs is to have click-on-an-item dismiss the
            // dialog instead of the user having to press 'Ok'.
            builder.setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    if(mEditText!=null){
                        try{
                            int value = Integer.parseInt(mEditText.getText().toString());
                            if (value >= m_min && value <= m_max){
                                final SeekbarPreference preference = getSeekbarPreference();
                                if(preference.callChangeListener(value))
                                    preference.setValue(value);
                            }
                        }catch(Exception e){
                            Log.e("SeekbarPreference",e.toString());
                        }
                    }
                    dialog.dismiss();
                }
            });
        }

        @Override
        public void onDialogClosed(boolean positiveResult) {
            if (positiveResult) {
                try{
                    int value = Integer.parseInt(mEditText.getText().toString());
                    if (value >= m_min && value <= m_max){
                        final SeekbarPreference preference = getSeekbarPreference();
                        if(preference.callChangeListener(value))
                            preference.setValue(value);
                    }
                }catch(Exception e){
                    Log.e("SeekbarPreference",e.toString());
                }
            }
        }
    }
    int mSeekBarValue;
    @SuppressWarnings("WeakerAccess") /* synthetic access */
            int mMin;
    private int mMax;
    private int mSeekBarIncrement;
    @SuppressWarnings("WeakerAccess") /* synthetic access */
            boolean mTrackingTouch;
    @SuppressWarnings("WeakerAccess") /* synthetic access */
            SeekBar mSeekBar;
    private TextView mSeekBarValueTextView;
    // Whether the SeekBar should respond to the left/right keys
    @SuppressWarnings("WeakerAccess") /* synthetic access */
            boolean mAdjustable;
    // Whether to show the SeekBar value TextView next to the bar
    private boolean mShowSeekBarValue;
    // Whether the SeekBarPreference should continuously save the Seekbar value while it is being
    // dragged.
    @SuppressWarnings("WeakerAccess") /* synthetic access */
            boolean mUpdatesContinuously;

    private final SeekBar.OnSeekBarChangeListener mSeekBarChangeListener = new SeekBar.OnSeekBarChangeListener() {
        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            if (fromUser && (mUpdatesContinuously || !mTrackingTouch)) {
                syncValueInternal(seekBar);
            } else {
                // We always want to update the text while the seekbar is being dragged
                updateLabelValue(progress + mMin);
            }
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
            mTrackingTouch = true;
        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            mTrackingTouch = false;
            if (seekBar.getProgress() + mMin != mSeekBarValue) {
                syncValueInternal(seekBar);
            }
        }
    };
    /**
     * Listener reacting to the user pressing DPAD left/right keys if {@code
     * adjustable} attribute is set to true; it transfers the key presses to the {@link SeekBar}
     * to be handled accordingly.
     */
    private final View.OnKeyListener mSeekBarKeyListener = new View.OnKeyListener() {
        @Override
        public boolean onKey(View v, int keyCode, KeyEvent event) {
            if (event.getAction() != KeyEvent.ACTION_DOWN) {
                return false;
            }

            if (!mAdjustable && (keyCode == KeyEvent.KEYCODE_DPAD_LEFT
                    || keyCode == KeyEvent.KEYCODE_DPAD_RIGHT)) {
                // Right or left keys are pressed when in non-adjustable mode; Skip the keys.
                return false;
            }

            // We don't want to propagate the click keys down to the SeekBar view since it will
            // create the ripple effect for the thumb.
            if (keyCode == KeyEvent.KEYCODE_DPAD_CENTER || keyCode == KeyEvent.KEYCODE_ENTER) {
                return false;
            }

            if (mSeekBar == null) {
                return false;
            }
            return mSeekBar.onKeyDown(keyCode, event);
        }
    };

    public SeekbarPreference(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);

        setDialogLayoutResource(aenu.aps3e.R.layout.edit_seek_bar);
        TypedArray a = context.obtainStyledAttributes(
                attrs, R.styleable.SeekBarPreference, defStyleAttr, defStyleRes);

        // The ordering of these two statements are important. If we want to set max first, we need
        // to perform the same steps by changing min/max to max/min as following:
        // mMax = a.getInt(...) and setMin(...).
        mMin = a.getInt(R.styleable.SeekBarPreference_min, 0);
        setMax(a.getInt(R.styleable.SeekBarPreference_android_max, 100));
        setSeekBarIncrement(a.getInt(R.styleable.SeekBarPreference_seekBarIncrement, 0));
        mAdjustable = a.getBoolean(R.styleable.SeekBarPreference_adjustable, true);
        mShowSeekBarValue = a.getBoolean(R.styleable.SeekBarPreference_showSeekBarValue, false);
        mUpdatesContinuously = a.getBoolean(R.styleable.SeekBarPreference_updatesContinuously,
                false);
        a.recycle();
    }

    public SeekbarPreference(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {

        this(context, attrs, defStyleAttr, 0);
    }


    public SeekbarPreference(@NonNull Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, R.attr.seekBarPreferenceStyle);
    }

    public SeekbarPreference(@NonNull Context context) {
        this(context, null);
    }

    @Override
    public void onBindViewHolder(@NonNull PreferenceViewHolder holder) {
        super.onBindViewHolder(holder);
        holder.itemView.setOnKeyListener(mSeekBarKeyListener);
        mSeekBar = (SeekBar) holder.findViewById(R.id.seekbar);
        mSeekBarValueTextView = (TextView) holder.findViewById(R.id.seekbar_value);
        if (mShowSeekBarValue) {
            mSeekBarValueTextView.setVisibility(View.VISIBLE);
        } else {
            mSeekBarValueTextView.setVisibility(View.GONE);
            mSeekBarValueTextView = null;
        }

        if (mSeekBar == null) {
            //Log.e(TAG, "SeekBar view is null in onBindViewHolder.");
            return;
        }
        mSeekBar.setOnSeekBarChangeListener(mSeekBarChangeListener);
        mSeekBar.setMax(mMax - mMin);
        // If the increment is not zero, use that. Otherwise, use the default mKeyProgressIncrement
        // in AbsSeekBar when it's zero. This default increment value is set by AbsSeekBar
        // after calling setMax. That's why it's important to call setKeyProgressIncrement after
        // calling setMax() since setMax() can change the increment value.
        if (mSeekBarIncrement != 0) {
            mSeekBar.setKeyProgressIncrement(mSeekBarIncrement);
        } else {
            mSeekBarIncrement = mSeekBar.getKeyProgressIncrement();
        }

        mSeekBar.setProgress(mSeekBarValue - mMin);
        updateLabelValue(mSeekBarValue);
        mSeekBar.setEnabled(isEnabled());
    }

    @Override
    protected void onSetInitialValue(Object defaultValue) {
        if (defaultValue == null) {
            defaultValue = 0;
        }
        setValue(getPersistedInt((Integer) defaultValue));
    }

    @Override
    protected @Nullable Object onGetDefaultValue(@NonNull TypedArray a, int index) {
        return a.getInt(index, 0);
    }

    public int getMin() {
        return mMin;
    }

    /**
     * Sets the lower bound on the {@link SeekBar}.
     *
     * @param min The lower bound to set
     */
    public void setMin(int min) {
        if (min > mMax) {
            min = mMax;
        }
        if (min != mMin) {
            mMin = min;
            notifyChanged();
        }
    }

    public int getValue() {
        return mSeekBarValue;
    }

    /**
     * Sets the current progress of the {@link SeekBar}.
     *
     * @param seekBarValue The current progress of the {@link SeekBar}
     */
    public void setValue(int seekBarValue) {
        setValueInternal(seekBarValue, true);
    }
    public final void setSeekBarIncrement(int seekBarIncrement) {
        if (seekBarIncrement != mSeekBarIncrement) {
            mSeekBarIncrement = Math.min(mMax - mMin, Math.abs(seekBarIncrement));
            notifyChanged();
        }
    }

    private void setValueInternal(int seekBarValue, boolean notifyChanged) {
        if (seekBarValue < mMin) {
            seekBarValue = mMin;
        }
        if (seekBarValue > mMax) {
            seekBarValue = mMax;
        }

        if (seekBarValue != mSeekBarValue) {
            mSeekBarValue = seekBarValue;
            updateLabelValue(mSeekBarValue);
            persistInt(seekBarValue);
            if (notifyChanged) {
                notifyChanged();
            }
        }
    }
    public int getMax() {
        return mMax;
    }

    /**
     * Sets the upper bound on the {@link SeekBar}.
     *
     * @param max The upper bound to set
     */
    public final void setMax(int max) {
        if (max < mMin) {
            max = mMin;
        }
        if (max != mMax) {
            mMax = max;
            notifyChanged();
        }
    }

    void syncValueInternal(@NonNull SeekBar seekBar) {
        int seekBarValue = mMin + seekBar.getProgress();
        if (seekBarValue != mSeekBarValue) {
            if (callChangeListener(seekBarValue)) {
                setValueInternal(seekBarValue, false);
            } else {
                seekBar.setProgress(mSeekBarValue - mMin);
                updateLabelValue(mSeekBarValue);
            }
        }
    }

    /**
     * Attempts to update the TextView label that displays the current value.
     *
     * @param value the value to display next to the {@link SeekBar}
     */
    @SuppressWarnings("WeakerAccess") /* synthetic access */
    void updateLabelValue(int value) {
        if (mSeekBarValueTextView != null) {
            mSeekBarValueTextView.setText(String.valueOf(value));
        }
    }

    private static class SavedState extends BaseSavedState {
        public static final Parcelable.Creator<SavedState> CREATOR =
                new Parcelable.Creator<SavedState>() {
                    @Override
                    public SavedState createFromParcel(Parcel in) {
                        return new SavedState(in);
                    }

                    @Override
                    public SavedState[] newArray(int size) {
                        return new SavedState[size];
                    }
                };

        int mSeekBarValue;
        int mMin;
        int mMax;

        SavedState(Parcel source) {
            super(source);

            // Restore the click counter
            mSeekBarValue = source.readInt();
            mMin = source.readInt();
            mMax = source.readInt();
        }

        SavedState(Parcelable superState) {
            super(superState);
        }

        @Override
        public void writeToParcel(Parcel dest, int flags) {
            super.writeToParcel(dest, flags);

            // Save the click counter
            dest.writeInt(mSeekBarValue);
            dest.writeInt(mMin);
            dest.writeInt(mMax);
        }
    }
}
