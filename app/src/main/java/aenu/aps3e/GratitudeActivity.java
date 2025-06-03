
package aenu.aps3e;
import android.app.*;
import android.os.*;
import android.widget.*;

public class GratitudeActivity extends Activity
{
	//gratitude_content
	static String list="\n"
	
	+ " callmerabbitz\n"//
	+ " collazof\n"//
	+ " devyprasetyo33\n"
	+ " 再见某人\n"
	+ " 同人小说\n"
	+ " 糖ωσ心の爱\n"
	+ " 不表态不经手不参与\n"
	+ " VM GAMEDROID\n"
	+ " brothason\n"
	+ " gamerpro\n" //
	+ " geovanem5\n"
	+ " edjeffher33\n"
	+ " Chakiel Zero Android\n"//
	+ " Darwinp\n"//
	+ " geovanem5\n"
	+ " sarahi\n"
	+ " melkygt0\n"
	+ " Gratitud e\n"
	+ " mediafire40\n"
	+ " klekot\n"
	+ " agustocastillo101\n"
	+ " Paul\n"
	+ " bakerrichard69\n"
	+ " Bardok84\n"
	+ " wingcom007\n"
	+ " Max\n"
	+ " kelve.p\n"
	+ " Sophia\n"
	+ " gonzaloinversionista\n"
	+ " josekelvin482\n"
	+ " superfuffa87\n"
	+ " kim81austin\n"
	+ " jblanm005\n"
	+ " dlt31795\n"
	+ " XZeusZX\n"
	+ " neucorazaocleon98\n"
	+ " Kyujj17\n"
	+ " Ryan.p\n"
	+ " 妖妖\n"
	+ " 太空飞瓜\n"
	+ " 明\n"
	+ " 萌酱的小可爱\n"
	+ " edjeffher33\n"
	+ " fernandez21\n"
	+ " yamil\n"
	+ " matschilui2\n"
	+ " 超玩游戏盒\n"
	+ " 冰糖\n"
	+ " christopher\n"
	+ " dalelace\n"
	+ " sandroloez\n"
	+ " 石头\n"
	+ " 鑫晓宇\n"
	;
	
	public static String optimize_list(){
		String l= list.replace("\n","\n    *");
		l.substring(0,l.length()-1);
		return l+"\n\n";
	}

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		TextView  tv = new TextView(this);
		String text=getString(R.string.gratitude_content);
		text+=optimize_list();
        tv.setText(text);
		tv.setTextIsSelectable(true);
		tv.setLongClickable(true);

		ScrollView sv=new ScrollView(this);
		sv.addView(tv);
        setContentView(sv);
	}
}
