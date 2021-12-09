package com.gizwits.opensource.appkit.ColorfulLight;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.gizwits.opensource.appkit.CommonModule.GosBaseActivity;
import com.gizwits.opensource.appkit.ControlModule.GosDeviceControlActivity;
import com.gizwits.opensource.appkit.ControlModule.GosDeviceLedControlActivity;
import com.gizwits.opensource.appkit.R;
import com.hisilicion.histreaming.GizWifiDevice;
import com.hisilicion.histreaming.GizWifiSDK;

public class ManualControlActivity extends GosBaseActivity {
    private Button m_ledSwitchButton;
    Intent intent;
    private GizWifiDevice device;
    private String m_status = "off";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_manual_control);
        setActionBar(true, true, "Manual_Control_Module");
        initDevice();
        manual_led_control_init();
    }

    private String switchStatus (){
        String status = "off";
        if(m_status.equals("off")) {
            status = "red_on";
        }
        if(m_status.equals("red_on")){
            status = "green_on";
        }
        if (m_status.equals("green_on")){
            status = "blue_on";
        }
        if(m_status.equals("blue_on")) {
            status = "red_on";
        }
        return  status;
    }

    private  void setText() {
        if (m_status.equals("red_on")){
            m_ledSwitchButton.setText(R.string.red_on);
            m_ledSwitchButton.setBackgroundResource(R.drawable.bt_red);
        } else if(m_status.equals("green_on")){
            m_ledSwitchButton.setText(R.string.green_on);
            m_ledSwitchButton.setBackgroundResource(R.drawable.bt_green);
        } else if (m_status.equals("blue_on")) {
            m_ledSwitchButton.setText(R.string.blue_on);
            m_ledSwitchButton.setBackgroundResource(R.drawable.bt_blue);
        }
    }
    /* Manual_control_Module*/
    private void manual_led_control_init(){
        m_ledSwitchButton = (Button) findViewById(R.id.red_yellow_green_bt);//红/绿/蓝开关
        m_ledSwitchButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String status = switchStatus();
                int code = GizWifiSDK.sharedInstance().post(device.m_index, "cl_s", status);
                if (code == 0) {
                    m_status = status;
                    setText();
//                    Toast.makeText(ManualControlActivity.this,  m_status, 100).show();
                } else if (code == -2) {
//                Toast.makeText(ManualControlActivity.this, device.alias + " is Offline", 2000).show();
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