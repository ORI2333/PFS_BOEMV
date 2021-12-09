package com.hisilicion.histreaming;

public class LinkPlatform {
    static {
        System.loadLibrary("HiStreamingLinkLite_jni");
    }

    private long m_context = 0;

    //
    // LinkPlatform API
    //
    public native int open();
    public native int close();
    public native int discover(String type);
    public native QueryResult query(String type);
}
