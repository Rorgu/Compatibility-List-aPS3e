// SPDX-License-Identifier: WTFPL

package aenu.aps3e;

import android.view.KeyEvent;
import org.vita3k.emulator.overlay.*;

public class KeyMapConfig {
    public static final int[] DEFAULT_KEYMAPPERS = new int[]{
        KeyEvent.KEYCODE_DPAD_LEFT, 
        KeyEvent.KEYCODE_DPAD_UP,
        KeyEvent.KEYCODE_DPAD_RIGHT, 
        KeyEvent.KEYCODE_DPAD_DOWN, 
        99, 96, 97, 100, 102, 104,
		0, 103, 105, 0, 108, 109,
		0, 
    };

    public static final int[] KEY_NAMEIDS = new int[]{
		R.string.keymapper_l,
		R.string.keymapper_u,
		R.string.keymapper_r,
		R.string.keymapper_d,
		R.string.keymapper_square,
		R.string.keymapper_cross,
		R.string.keymapper_circle,
		R.string.keymapper_triangle,
    
		R.string.keymapper_l1,
		R.string.keymapper_l2,
		R.string.keymapper_l3,

		R.string.keymapper_r1,
		R.string.keymapper_r2,
		R.string.keymapper_r3,
		
		R.string.keymapper_start,
		R.string.keymapper_select,
		R.string.keymapper_ps,
        };

    public static final int[] KEY_VALUES = new int[]{
		InputOverlay.ControlId.l,
		InputOverlay.ControlId.u,
		InputOverlay.ControlId.r,
		InputOverlay.ControlId.d,
		InputOverlay.ControlId.square,
		InputOverlay.ControlId.cross,
		InputOverlay.ControlId.circle,
		InputOverlay.ControlId.triangle,
		
		InputOverlay.ControlId.l1,
		InputOverlay.ControlId.l2,
		InputOverlay.ControlId.l3,
		
		InputOverlay.ControlId.r1,
		InputOverlay.ControlId.r2,
		InputOverlay.ControlId.r3,
		
		InputOverlay.ControlId.start,
		InputOverlay.ControlId.select,
		
		InputOverlay.ControlId.ps,
	};
}
