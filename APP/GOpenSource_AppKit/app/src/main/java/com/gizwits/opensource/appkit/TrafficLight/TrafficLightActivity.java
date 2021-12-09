package com.gizwits.opensource.appkit.TrafficLight;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

import com.gizwits.opensource.appkit.ColorfulLight.ColorLightActivity;
import com.gizwits.opensource.appkit.ColorfulLight.ManualControlActivity;
import com.gizwits.opensource.appkit.ColorfulLight.ReturnMenuActivity;
import com.gizwits.opensource.appkit.CommonModule.GosBaseActivity;
import com.gizwits.opensource.appkit.R;
import com.hisilicion.histreaming.GizWifiDevice;

public class TrafficLightActivity extends GosBaseActivity {
    private Button m_Traffic_Manual_Control_Module_Button;
    private Button m_Auto_Module_Button;
    private Button m_Human_Module_Button;
    private Button m_Return_Menu_Module_Button;
    Intent intent;
    private GizWifiDevice device;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_traffic_light);
        initDevice();
        setActionBar(true, true, "Traffic_Light");
        traffic_light_view_init();
    }

    private void initDevice() {
        Intent intent = getIntent();
        device = (GizWifiDevice) intent.getParcelableExtra("GizWifiDevice");
    }
    private void traffic_light_view_init() {
        m_Traffic_Manual_Control_Module_Button = (Button)findViewById(R.id.Traffic_Manual_Control_Module_bt);
        m_Auto_Module_Button = (Button)findViewById(R.id.Traffic_Auto_Module_bt);
        m_Human_Module_Button =(Button)findViewById(R.id.Traffic_Human_Module_bt);
        m_Return_Menu_Module_Button =(Button)findViewById(R.id.Traffic_Return_Menu_Module_bt);
        /*traffic light 模式下手动控制模式*/
        m_Traffic_Manual_Control_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(TrafficLightActivity.this, TrafficManualControlActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
        /*traffic light 模式下自动控制模式*/
        m_Auto_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(TrafficLightActivity.this, TrafficAutoModuleActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
        /*traffic light 模式下人工控制模式*/
        m_Human_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(TrafficLightActivity.this, TrafficHumanModuleActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
        /*traffic light 模式下返回界面*/
        m_Return_Menu_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(TrafficLightActivity.this, TrafficLightReturnMenuActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
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