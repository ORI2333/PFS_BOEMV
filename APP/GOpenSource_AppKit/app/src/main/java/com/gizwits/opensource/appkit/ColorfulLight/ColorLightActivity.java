package com.gizwits.opensource.appkit.ColorfulLight;

import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

import com.gizwits.opensource.appkit.CommonModule.GosBaseActivity;
import com.gizwits.opensource.appkit.R;
import com.hisilicion.histreaming.GizWifiDevice;

public class ColorLightActivity extends GosBaseActivity {
    private Button m_Manual_Control_Module_Button;
    private Button m_Colorful_Light_Module_Button;
    private Button m_PWMC_Control_Module_Button;
    private Button m_Brightness_Module_Button;
    private Button m_Human_Detect_Module_Button;
    private Button m_Light_Detect_Module_Button;
    private Button m_Union_Detect_Module_Button;
    private Button m_Return_Menu_Module_Button;
    Intent intent;
    private GizWifiDevice device;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_color_light);
        initDevice();
        setActionBar(true, true, "Colorful_light");
        colorful_light_view_init();
    }

    private void initDevice() {
        Intent intent = getIntent();
        device = (GizWifiDevice) intent.getParcelableExtra("GizWifiDevice");
    }

    private void colorful_light_view_init() {
        m_Manual_Control_Module_Button = (Button)findViewById(R.id.Manual_Control_Module_bt);
        m_Colorful_Light_Module_Button = (Button)findViewById(R.id.Colorful_Light_Module_bt);
        m_PWMC_Control_Module_Button = (Button)findViewById(R.id.PWM_Control_Module_bt);
        m_Brightness_Module_Button = (Button)findViewById(R.id.Brightness_Module_bt);
        m_Human_Detect_Module_Button = (Button)findViewById(R.id.Human_Detect_Module_bt);
        m_Light_Detect_Module_Button = (Button)findViewById(R.id.Light_Detect_Module_bt);
        m_Union_Detect_Module_Button = (Button)findViewById(R.id.Union_Detect_Module_bt);
        m_Return_Menu_Module_Button = (Button)findViewById(R.id.Return_Menu_Module_bt);
        /*colorful light 模式下的手动按键模式*/
        m_Manual_Control_Module_Button.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                intent = new Intent(ColorLightActivity.this, ManualControlActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
        /*colorful light 模式下的手动周期模式*/
        m_Colorful_Light_Module_Button.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                intent = new Intent(ColorLightActivity.this, periodFlashActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
        /*colorful light 模式下pwm 控制模式*/
        m_PWMC_Control_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(ColorLightActivity.this, pwmControlActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
        /*colorful light 模式下亮度调节*/
        m_Brightness_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(ColorLightActivity.this, BrightnessActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
        /*colorful light 模式下人体检测*/
        m_Human_Detect_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(ColorLightActivity.this, Human_DetectActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
        /*colorful light 模式下光照检测*/
        m_Light_Detect_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(ColorLightActivity.this, LightDetectActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
        /*colorful light 模式下联合检测(人体sensor和光敏sensor)*/
        m_Union_Detect_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(ColorLightActivity.this, UnionActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
        /*colorful light 模式下返回界面*/
        m_Return_Menu_Module_Button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(ColorLightActivity.this, ReturnMenuActivity.class);
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