package java.lang;

/**
 *
 * @author igel
 */
public class Runtime {

    private static final Runtime RUNTIME = new Runtime();

    private Runtime() {
    }

    public static Runtime getRuntime() {
        return RUNTIME;
    }

    public native long freeMemory();

    public native long maxMemory();
//    public native long totalMemory();
}
