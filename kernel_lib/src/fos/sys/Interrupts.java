package fos.sys;

/**
 * Работа с прерываниями
 * @author igel
 */
public class Interrupts {

    /**
     * Разрешает прерывания
     */
    public static native void enable();

    /**
     * Запрещает прерывания
     */
    public static native void disable();

    /**
     * Ожидает прерывание
     */
    public static native void halt();
}
