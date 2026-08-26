"use client";

import { Badge } from "@/components/ui/badge";
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";

const plans = [
  {
    id: "a",
    label: "方案 A",
    title: "双 MCU 三模 1k（推荐）",
    keep: "保留 GP2040-CE",
    effort: "中",
    body: "RP2040 继续读键、跑 SOCD / 涡轮 / Web 配置 / PS5 USB 认证。nRF52840（或 ESP32-S3）通过 UART 拿 GamepadState，负责 BLE 和 2.4G。电脑侧再做一颗同芯片的接收器，枚举成 XInput/HID。",
    steps: [
      "有线路径一行都不用改，插上还是现在的 1 kHz 手柄。",
      "无线 MCU 只当「按键状态的射频网卡」，不要重做格斗逻辑。",
      "2.4G 用 Nordic ESB，比 BLE HID 稳；BLE 留给手机 / Switch 2 这类能认 HID 的设备。",
      "接收器是第二份固件，不是把手柄直接当 BLE 外设就能当 1k 竞技设备。",
    ],
    pipe: ["按键", "RP2040 GP2040-CE", "USB 有线 1k · 主机认证", "nRF52840", "BLE / ESB 2.4G", "USB 接收器"],
  },
  {
    id: "b",
    label: "方案 B",
    title: "单芯片 nRF52840 三模 1k",
    keep: "放弃 GP2040-CE",
    effort: "高",
    body: "用 nRF Connect SDK + TinyUSB 重写手柄。一块芯片同时 USB Device、BLE、ESB。Web 配置、14 种输入模式、PS5 认证、RGB、摇杆全部要自己搬。",
    steps: [
      "PC 有线 / 2.4G 1k 最干净，BOM 也最省。",
      "PS5 / Xbox 官方认证几乎做不到，除非再挂一颗 RP2040 只做认证——那就变回方案 A。",
      "Switch 可以用 BLE 或 USB，和现成 GP2040 模式不是同一套描述符。",
    ],
    pipe: ["按键", "nRF52840", "USB FS 1k", "BLE HID", "ESB → 接收器"],
  },
  {
    id: "c",
    label: "方案 C",
    title: "有线 8k 新固件",
    keep: "放弃 GP2040-CE",
    effort: "高",
    body: "换 Teensy 4.1 / STM32H743 / CH32V307。USB 描述符改成 High Speed，bInterval=1 才是 125 μs。只对 PC 有意义。",
    steps: [
      "先在 Teensy 4.1 上做最小 HID 手柄，用 USBPcap / 轮询检测工具确认真 8000 Hz。",
      "再移植按键矩阵、SOCD。不要指望 Pico SDK 工程能交叉编译。",
      "主机模式、认证狗、Web 配置这些 GP2040 功能默认全部丢掉。",
    ],
    pipe: ["按键", "USB HS MCU", "微帧 125 μs", "PC 8 kHz"],
  },
  {
    id: "d",
    label: "方案 D",
    title: "三模 8k 整机",
    keep: "新产品",
    effort: "很高",
    body: "手柄侧 USB HS MCU + 2.4G 射频；接收器侧也必须是 USB HS。BLE 到不了 8k，只能当兼容模式。这是鼠标厂的 Lightspeed / HyperSpeed 级别工程。",
    steps: [
      "2.4G 8k 意味着每 125 μs 一包还要 ACK，协议、天线、共存都要重新做。",
      "Windows 认 8k HID 没问题；PS5 / Switch 不会按 8k 轮询。",
      "和现仓库没有可复用的 USB / 射频代码。",
    ],
    pipe: ["按键", "HS MCU + nRF", "私有 2.4G 8k", "HS USB 接收器", "PC"],
  },
];

export function ArchitectureTabs() {
  return (
    <Tabs defaultValue="a">
      <TabsList variant="line" className="w-full flex-wrap justify-start">
        {plans.map((plan) => (
          <TabsTrigger key={plan.id} value={plan.id}>
            {plan.label}
          </TabsTrigger>
        ))}
      </TabsList>
      {plans.map((plan) => (
        <TabsContent key={plan.id} value={plan.id} className="mt-4">
          <Card>
            <CardHeader>
              <div className="flex flex-wrap items-center gap-2">
                <CardTitle>{plan.title}</CardTitle>
                <Badge variant="outline">{plan.keep}</Badge>
                <Badge variant="secondary">工程量 {plan.effort}</Badge>
              </div>
              <CardDescription className="max-w-3xl text-pretty leading-6">
                {plan.body}
              </CardDescription>
            </CardHeader>
            <CardContent className="space-y-5">
              <Pipeline steps={plan.pipe} />
              <ul className="space-y-2 text-sm leading-6 text-muted-foreground">
                {plan.steps.map((step) => (
                  <li key={step} className="flex gap-2">
                    <span className="mt-2 size-1.5 shrink-0 rounded-full bg-cyan-400" />
                    <span>{step}</span>
                  </li>
                ))}
              </ul>
            </CardContent>
          </Card>
        </TabsContent>
      ))}
    </Tabs>
  );
}

function Pipeline({ steps }: { steps: string[] }) {
  return (
    <div className="flex flex-wrap items-center gap-2">
      {steps.map((step, i) => (
        <div key={step} className="flex items-center gap-2">
          <span className="rounded-md border border-border bg-muted/60 px-2.5 py-1 text-xs font-medium">
            {step}
          </span>
          {i < steps.length - 1 ? (
            <span className="text-muted-foreground">→</span>
          ) : null}
        </div>
      ))}
    </div>
  );
}
