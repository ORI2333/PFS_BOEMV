package com.gizwits.opensource.appkit.ColorfulLight;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import com.gizwits.opensource.appkit.CommonModule.GosBaseActivity;
import com.gizwits.opensource.appkit.R;

public class ManualLedControlActivity extends GosBaseActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_manual_led_control);
    }
}