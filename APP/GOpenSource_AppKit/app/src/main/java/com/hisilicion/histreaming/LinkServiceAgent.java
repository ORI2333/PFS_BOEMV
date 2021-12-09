package com.hisilicion.histreaming;

public class LinkServiceAgent {
    private long m_context = 0;

    public native String type();
    public native String address();
    public native String get(String property);
    public native int modify(String property, String v);
    public native int id();
}
