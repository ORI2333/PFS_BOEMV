package com.hisilicion.histreaming;

import java.util.ArrayList;

public class QueryResult {
    private long m_context;

    public native int count ();
    public native LinkServiceAgent at(int index);
    public native void free();
}
