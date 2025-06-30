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
		R.string.left,
		R.string.up,
		R.string.right,
		R.string.down,
		R.string.square,
		R.string.cross,
		R.string.circle,
		R.string.triangle,
    
		R.string.l1,
		R.string.l2,
		R.string.l3,

		R.string.r1,
		R.string.r2,
		R.string.r3,
		
		R.string.start,
		R.string.select,
		R.string.ps,
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
