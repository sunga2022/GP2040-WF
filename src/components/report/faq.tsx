"use client";

import {
  Accordion,
  AccordionItem,
  AccordionTrigger,
  AccordionContent,
} from "@/components/ui/accordion";
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";

const faqs = [
  {
    q: "把 bInterval 改成更小的数，能不能到 8k？",
    a: "不能。USB Full Speed 的时间单位就是 1 ms。描述符里的 1 已经是最小值。High Speed 上同一个数字才变成 125 μs。RP2040 没有 HS PHY，改数字只会让设备枚举异常。",
  },
  {
    q: "Pico W 不是有蓝牙吗？",
    a: "有，CYW43439 支持经典蓝牙和 BLE。GP2040-CE 完全没接这套。即便接上 BTstack，HID 报告间隔抖动很大，官方 issue 里也有人测过不稳定。它也没有可对标鼠标接收器的 2.4G 私有协议，不适合格斗板。",
  },
  {
    q: "换 Pico 2 / RP2350 能 8k 吗？",
    a: "不能。RP2350 USB 仍然是 Full Speed。CPU 更快只让你有更多时间跑 RGB 和显示，改变不了 1 ms 帧。",
  },
  {
    q: "2.4G 用 nRF24L01 模块行不行？",
    a: "能做低速率遥控，很难做稳定 1 kHz 手柄，更别说 8k。要做产品级 2.4G，用 nRF52840 的 ESB / Gazell，接收器也用同系列芯片。nRF24 没有 USB，接收器还得再找一颗 USB MCU。",
  },
  {
    q: "蓝牙能不能 8k？",
    a: "标准 BLE HID 连接间隔最快大约 7.5 ms（约 133 Hz）。厂商宣传的「BLE 1k / 8k」几乎都是私有协议，不是系统 HID。Windows / 主机不会按 8 kHz 去轮询一颗普通 BLE 手柄。",
  },
  {
    q: "我的 PS5 USB 认证还能留吗？",
    a: "只有继续让 RP2040 当有线主控才最省事。你现在的 G2 / RUYI / doio 都把 PIO-USB D+ 拉出来接认证手柄。换到 nRF 或 STM32 之后，这套认证栈没有对应实现。三模产品的务实做法：插线走 GP2040-CE，拔线走 nRF。",
  },
  {
    q: "格斗游戏上 8k 有没有手感？",
    a: "几乎没有。SF6 / 拳皇 / 街霸类 60 帧，一帧 16.7 ms。1 kHz 已经把按键送到帧内很靠前的位置。8 kHz 把平均等待从 0.5 ms 降到 0.06 ms，显示器和游戏逻辑都分辨不出来。它主要出现在 FPS 鼠标参数表上。",
  },
  {
    q: "如果只做 PC 三模 1k，最小闭环是什么？",
    a: "一块 XIAO nRF52840 当手柄，再买一块当接收器。USB 枚举 XInput 或标准游戏手柄，2.4G 用 ESB 传 8–20 字节按键包，BLE 做备用。按键先用杜邦线接几个开关验证延迟，再接到你现有摇杆板的空闲排针。",
  },
];

export function Faq() {
  return (
    <Card>
      <CardHeader>
        <CardTitle>常见误区</CardTitle>
        <CardDescription>
          这些是改 GP2040-CE 时最容易踩的结论。每一条都对应仓库现状或 USB 规范，不是「再优化一下固件」能绕过去的。
        </CardDescription>
      </CardHeader>
      <CardContent>
        <Accordion>
          {faqs.map((item) => (
            <AccordionItem key={item.q} value={item.q}>
              <AccordionTrigger className="text-left">{item.q}</AccordionTrigger>
              <AccordionContent>
                <p className="leading-6 text-muted-foreground">{item.a}</p>
              </AccordionContent>
            </AccordionItem>
          ))}
        </Accordion>
      </CardContent>
    </Card>
  );
}
