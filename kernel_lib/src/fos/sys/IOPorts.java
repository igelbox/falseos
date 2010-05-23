package fos.sys;

/**
 * Работа с портами ввода-вывода
 * @author igel
 */
public class IOPorts {

    /**
     * Получает значение из заданного порта ввода
     * @param port № порта
     * @return прочитанное значение из порта ввода
     */
    public static native int in( int port );

    /**
     * Записывает значение в заданный порт вывода
     * @param port № порта
     * @param value записываемое значение
     */
    public static native void out( int port, int value );
}
