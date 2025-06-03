package aenu.lang;

public class System {
    static {
        java.lang.System.loadLibrary("lang_System");
    }
    public static native void setenv(String name,String value,boolean overwrite);
    public static void setenv(String name,String value){
        setenv(name,value,true);
    }

}
