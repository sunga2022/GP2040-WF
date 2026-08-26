import { Badge } from "@/components/ui/badge";
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import { Bluetooth, Cable, Radio } from "lucide-react";

const verdicts = [
  {
    icon: Radio,
    title: "三模：不能在现固件上「加一加」",
    tone: "warn" as const,
    body: "仓库里没有任何蓝牙、BLE、nRF24、ESB 或 2.4G 驱动。RP2040 也没有射频。能做的是外挂一颗无线 MCU，用 UART/SPI 把按键状态送出去，有线部分继续跑 GP2040-CE。",
  },
  {
    icon: Cable,
    title: "8 kHz：改 bInterval 没用",
    tone: "bad" as const,
    body: "XInput / Switch / HID 描述符里已经是 bInterval = 1。在 USB Full Speed 上这就是 1 ms 一帧，也就是 1000 Hz 的物理上限。要 8 kHz 必须换带 USB High Speed 的芯片，GP2040-CE 整份 USB 栈都要换掉。",
  },
  {
    icon: Bluetooth,
    title: "格斗板更该守 1 kHz 三模",
    tone: "good" as const,
    body: "主机（PS5 / Switch）不会吃 8 kHz。60 帧格斗游戏一帧 16.7 ms，你现在 XInput 平均 0.76 ms 已经远低于一帧。8 kHz 主要是 PC 鼠标营销；手柄上性价比极差。",
  },
];

const toneClass = {
  good: "border-emerald-500/30 bg-emerald-500/8",
  warn: "border-amber-500/30 bg-amber-500/8",
  bad: "border-rose-500/30 bg-rose-500/8",
};

const badgeLabel = {
  good: "建议方向",
  warn: "架构要拆",
  bad: "硬件墙",
};

export function VerdictCards() {
  return (
    <div className="grid gap-4 md:grid-cols-3">
      {verdicts.map((item) => (
        <Card key={item.title} className={toneClass[item.tone]}>
          <CardHeader>
            <div className="flex items-center justify-between gap-2">
              <item.icon className="size-5 text-foreground" />
              <Badge variant="outline">{badgeLabel[item.tone]}</Badge>
            </div>
            <CardTitle className="text-base leading-6">{item.title}</CardTitle>
            <CardDescription className="text-pretty leading-6">
              {item.body}
            </CardDescription>
          </CardHeader>
          <CardContent />
        </Card>
      ))}
    </div>
  );
}
