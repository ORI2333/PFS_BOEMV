package com.gizwits.opensource.appkit.ControlModule;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.gizwits.opensource.appkit.ColorfulLight.ColorLightActivity;
import com.gizwits.opensource.appkit.CommonModule.GosBaseActivity;
import com.gizwits.opensource.appkit.Environment.EnvironmentActivity;
import com.gizwits.opensource.appkit.NFC.NFC_ControlActivity;
import com.gizwits.opensource.appkit.NFC.NFC_WeChatActivity;
import com.gizwits.opensource.appkit.R;
import com.gizwits.opensource.appkit.TrafficLight.TrafficLightActivity;
import com.hisilicion.histreaming.GizWifiDevice;
import com.hisilicion.histreaming.GizWifiSDK;

public class GosControlActivity extends GosBaseActivity {

    //private Button m_ledControlButton;
    private Button m_carControlButton;
    //private Button m_modularControlButton;
    //private Button m_colorfulLightButton;
    //private Button m_trafficLightButton;
    //private Button m_environmentButton;
    //private Button m_nfcButton;
    Intent intent;
    private GizWifiDevice device;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_gos_control);
        initDevice();
        initView();
    }
    private void initDevice() {
        Intent intent = getIntent();
        device = (GizWifiDevice) intent.getParcelableExtra("GizWifiDevice");
    }

    private void initView() {
//        String status = "return";
//        int code = GizWifiSDK.sharedInstance().post(device.m_index, "histreaming_return_main_menu_status", status);
//        if (code == 0) {
//            Toast.makeText(GosControlActivity.this, device.alias + " return main menu", 1000).show();
//        }
        //m_ledControlButton = (Button)findViewById(R.id.led_demo_control_bt);
        m_carControlButton = (Button)findViewById(R.id.car_control_bt);
        //m_modularControlButton = (Button)findViewById(R.id.modular_control_bt);
        //m_colorfulLightButton = (Button)findViewById(R.id.colorful_light_bt);
        //m_trafficLightButton = (Button)findViewById(R.id.traffic_light_bt);
        //m_environmentButton = (Button)findViewById(R.id.environment_bt) ;
        //m_nfcButton = (Button)findViewById(R.id.nfc_bt);

//        m_ledControlButton.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View view) {
//                intent = new Intent(GosControlActivity.this, GosDeviceControlActivity.class);
//                Bundle bundle = new Bundle();
//                bundle.putParcelable("GizWifiDevice", device);
//                intent.putExtras(bundle);
//                startActivityForResult(intent, 1);
//            }
//        });
//
        m_carControlButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                intent = new Intent(GosControlActivity.this, GosCarControlActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("GizWifiDevice", device);
                intent.putExtras(bundle);
                startActivityForResult(intent, 1);
            }
        });
//
//        m_modularControlButton.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View view) {
//                intent = new Intent(GosControlActivity.this, GosModularControlActivity.class);
//                Bundle bundle = new Bundle();
//                bundle.putParcelable("GizWifiDevice", device);
//                intent.putExtras(bundle);
//                startActivityForResult(intent, 1);
//            }
//        });
//        /*colorful light*/
//        m_colorfulLightButton.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View view) {
//                intent = new Intent(GosControlActivity.this, ColorLightActivity.class);
//                Bundle bundle = new Bundle();
//                bundle.putParcelable("GizWifiDevice", device);
//                intent.putExtras(bundle);
//                startActivityForResult(intent, 1);
//            }
//        });
//        /*traffic light*/
//        m_trafficLightButton.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View view) {
//                intent = new Intent(GosControlActivity.this, TrafficLightActivity.class);
//                Bundle bundle = new Bundle();
//                bundle.putParcelable("GizWifiDevice", device);
//                intent.putExtras(bundle);
//                startActivityForResult(intent, 1);
//            }
//        });
//        /*environment test*/
//        m_environmentButton.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View view) {
//                intent = new Intent(GosControlActivity.this, EnvironmentActivity.class);
//                Bundle bundle = new Bundle();
//                bundle.putParcelable("GizWifiDevice", device);
//                intent.putExtras(bundle);
//                startActivityForResult(intent, 1);
//            }
//        });
//        /*NFC test*/
//        m_nfcButton.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View view) {
//                intent = new Intent(GosControlActivity.this, NFC_ControlActivity.class);
//                Bundle bundle = new Bundle();
//                bundle.putParcelable("GizWifiDevice", device);
//                intent.putExtras(bundle);
//                startActivityForResult(intent, 1);
//            }
//        });
    }
}
