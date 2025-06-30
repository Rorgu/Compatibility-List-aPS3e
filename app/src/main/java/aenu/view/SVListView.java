package aenu.view;

import android.widget.ListView;

public class SVListView extends ListView {
    public SVListView(android.content.Context context, android.util.AttributeSet attrs) {
        super(context, attrs);
    }
    public SVListView(android.content.Context context) {
        super(context);
    }
    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        int expandSpec = MeasureSpec.makeMeasureSpec(Integer.MAX_VALUE >> 2, MeasureSpec.AT_MOST);
        super.onMeasure(widthMeasureSpec, expandSpec);
    }
}
