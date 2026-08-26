import { Badge } from "@/components/ui/badge";
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import { customBoards, inputModes } from "@/lib/report-data";
import { Check, Minus } from "lucide-react";

const rows = [
  {
    item: "USB 设备栈",
    status: "yes" as const,
    detail: "TinyUSB Device，Core0 主循环里 tud_task() + 各平台 Driver",
  },
  {
    item: "USB 主机栈",
    status: "yes" as const,
    detail: "Pico-PIO-USB，给 PS5 / Xbox 认证狗和手柄转接，不是无线",
  },
  {
    item: "输入模式",
    status: "yes" as const,
    detail: inputModes.join(" · "),
  },
  {
    item: "轮询请求",
    status: "yes" as const,
    detail: "主流模式 bInterval=1 → Full Speed 上即 1000 Hz",
  },
  {
    item: "蓝牙 / BLE",
    status: "no" as const,
    detail: "源码与 submodule 均无 BTstack、NimBLE、CYW43 HID",
  },
  {
    item: "2.4G 私有协议",
    status: "no" as const,
    detail: "无 nRF24、ESB、ESP-NOW、Gazell、无线接收器固件",
  },
  {
    item: "USB High Speed",
    status: "no" as const,
    detail: "tusb_config.h 对 RP2040 固定 OPT_MODE_FULL_SPEED",
  },
];

export function FirmwareScan() {
  return (
    <div className="grid gap-4 lg:grid-cols-[1.2fr_0.8fr]">
      <Card>
        <CardHeader>
          <CardTitle>对着 sunga2022/GP2040-CE 扫过的结果</CardTitle>
          <CardDescription>
            这是 OpenStickCommunity/GP2040-CE 的衍生品，主循环、驱动和 USB
            描述符都还是有线格斗板架构。你加的是板级引脚，不是无线协议。
          </CardDescription>
        </CardHeader>
        <CardContent>
          <ul className="divide-y divide-border">
            {rows.map((row) => (
              <li
                key={row.item}
                className="flex flex-col gap-1 py-3 sm:flex-row sm:items-start sm:gap-4"
              >
                <div className="flex w-40 shrink-0 items-center gap-2 text-sm font-medium">
                  {row.status === "yes" ? (
                    <Check className="size-4 text-emerald-400" />
                  ) : (
                    <Minus className="size-4 text-rose-400" />
                  )}
                  {row.item}
                </div>
                <p className="text-sm leading-6 text-muted-foreground">
                  {row.detail}
                </p>
              </li>
            ))}
          </ul>
        </CardContent>
      </Card>
      <Card>
        <CardHeader>
          <CardTitle>仓库里的自定义板</CardTitle>
          <CardDescription>
            configs/ 下 17 套都是 RP2040 GPIO
            映射。G2、RUYI、doio 等都开了 USB 从口做主机认证，全部没有射频引脚。
          </CardDescription>
        </CardHeader>
        <CardContent className="flex flex-wrap gap-2">
          {customBoards.map((name) => (
            <Badge key={name} variant="secondary">
              {name}
            </Badge>
          ))}
        </CardContent>
      </Card>
    </div>
  );
}
