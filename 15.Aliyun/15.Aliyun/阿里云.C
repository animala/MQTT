
{
  "ProductKey": "a1ugBNniFGU",
  "DeviceName": "FY-STM32",
  "DeviceSecret": "0ywar1aLJfsRE6njaB0qu9nK8oqqS0yo"
}
设备的SN：123456 //这里的SN是生成MQTT连接密码用到的，在真实的设备可以用UUID，这里就简单取一个

/sys/a1ugBNniFGU/FY-STM32/thing/event/property/post			发布
/sys/a1ugBNniFGU/FY-STM32/thing/service/property/set		订阅

地域字符串：cn-shanghai
连接域名：a1ugBNniFGU.iot-as-mqtt.cn-shanghai.aliyuncs.com:1883

Client ID：123456|securemode=3,signmethod=hmacsha1,timestamp=789|

用户名：	FY-STM32&a1ugBNniFGU

密钥：b447b9f26938d8eba6b2a4878066aae91839600c
mqttPassword=hmacsha1("0ywar1aLJfsRE6njaB0qu9nK8oqqS0yo","clientId123456deviceNameFY-STM32productKeya1ugBNniFGUtimestamp789").toHexString();

温度关键字：CurrentTemperature
颜色关键字：hue
{
  "method" : "thing.service.property.set",
  "id" : "630262306",
  "params" : {
    "CurrentTemperature" : 12.6,
    "hue" : 0
  },
  "version" : "1.0.0"
}


{\"method\":\"thing.service.property.set\",\"id\":\"630262306\",\"params\":{\
	\"CurrentTemperature\":12.6,\
	\"hue\":0,\
	\"mlux\":0\
},\"version\":\"1.0.0\"}





