package com.gizwits.opensource.appkit.ControlModule;

import android.content.Intent;
import android.support.v7.app.ActionBar;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.gizwits.opensource.appkit.CommonModule.GosBaseActivity;
import com.gizwits.opensource.appkit.R;
import com.hisilicion.histreaming.GizWifiDevice;
import com.hisilicion.histreaming.GizWifiSDK;

public class GosDeviceLedControlActivity  extends GosBaseActivity {
    /** The tv MAC */
    private Button m_switchButton;
    private Button m_slowButton;
    private Button m_fastButton;

    /** The GizWifiDevice device */
    private GizWifiDevice device;
    private String m_status = "Off";

    /** The ActionBar actionBar */
    ActionBar actionBar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_gos_device_led_control);
        initDevice();
        setActionBar(true, true, "LedControl");
        initView();
    }

    private  String switchStatus(){
        String status = "Off";
        if(m_status.equals("Off") || m_status.equals("Slow") || m_status.equals("Fast")){
            status = "On";
        }else if(m_status.equals("On") || m_status.equals("Slow") || m_status.equals("Fast")){
            status = "Off";
        }
        return  status;
    }

    private  void setText() {
        if(m_status.equals("On")){
            m_switchButton.setText(R.string.off);
        }else if(m_status.equals("Off")){
            m_switchButton.setText(R.string.on);
        }
    }

    private void initView() {
        m_switchButton = (Button) findViewById(R.id.switch_bt);//开关按钮
        m_slowButton = (Button) findViewById(R.id.slow_bt);    //慢闪按钮
        m_fastButton = (Button) findViewById(R.id.fast_bt);    //快闪按钮

        m_switchButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String status = switchStatus();
                int code = GizWifiSDK.sharedInstance().post(device.m_index, "status", status);
                if (code == 0) {
                    m_status = status;
                    setText();
                    Toast.makeText(GosDeviceLedControlActivity.this,  "wgm"+m_status, 100).show();//add by wgm
                } else if (code == -2) {
                    Toast.makeText(GosDeviceLedControlActivity.this, device.alias + " is Offline", 2000).show();
                }
            }
        });

        m_slowButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(m_status.equals("On") || m_status.equals("Off") || m_status.equals("Fast") ) {
                    int code = GizWifiSDK.sharedInstance().post(device.m_index, "status", "Slow");
                    if (code == 0) {
                        m_status = "Slow";
                        Toast.makeText(GosDeviceLedControlActivity.this, "wgm" + m_status, 100).show();//add by wgm
                    } else if (code == -2) {
                        Toast.makeText(GosDeviceLedControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                }
            }
        });

        m_fastButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(m_status.equals("On") || m_status.equals("Off") || m_status.equals("Slow") ) {
                    int code = GizWifiSDK.sharedInstance().post(device.m_index, "status", "Fast");
                    if (code == 0) {
                        m_status = "Fast";
                        Toast.makeText(GosDeviceLedControlActivity.this, "wgm" + m_status, 100).show();//add by wgm
                    } else if (code == -2) {
                        Toast.makeText(GosDeviceLedControlActivity.this, device.alias + " is Offline", 2000).show();
                    }
                }
            }
        });
    }

    private void initDevice() {
        Intent intent = getIntent();
        device = (GizWifiDevice) intent.getParcelableExtra("GizWifiDevice");
    }
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                this.finish();
                break;
        }
        return super.onOptionsItemSelected(item);
    }

}
