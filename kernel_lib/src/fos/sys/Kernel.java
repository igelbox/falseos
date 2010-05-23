package fos.sys;

/**
 *
 * @author igel
 */
public final class Kernel {

    private static Kernel instance;

    private Kernel() {
    }

    public static void init() {
        if ( instance != null )
            return;
        instance = new Kernel();
    }
}
