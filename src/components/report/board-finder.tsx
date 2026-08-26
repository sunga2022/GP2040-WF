"use client";

import { useMemo, useState } from "react";
import { Badge } from "@/components/ui/badge";
import { Button } from "@/components/ui/button";
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeader,
  TableRow,
} from "@/components/ui/table";
import { boards, goals, type GoalId } from "@/lib/report-data";

export function BoardFinder() {
  const [goal, setGoal] = useState<GoalId | "all">("all");

  const visible = useMemo(
    () =>
      goal === "all" ? boards : boards.filter((board) => board.goals.includes(goal)),
    [goal]
  );

  return (
    <Card>
      <CardHeader>
        <CardTitle>开发板对照</CardTitle>
        <CardDescription>
          按目标筛。标了「首选」的是评估阶段最省事的现货板，不是最终量产 BOM。
        </CardDescription>
      </CardHeader>
      <CardContent className="space-y-4">
        <div className="flex flex-wrap gap-2">
          <Button
            size="sm"
            variant={goal === "all" ? "default" : "outline"}
            onClick={() => setGoal("all")}
          >
            全部
          </Button>
          {goals.map((item) => (
            <Button
              key={item.id}
              size="sm"
              variant={goal === item.id ? "default" : "outline"}
              onClick={() => setGoal(item.id)}
            >
              {item.label}
            </Button>
          ))}
        </div>
        <p className="text-sm text-muted-foreground">
          {goal === "all"
            ? "13 块评估方向。先定目标再买板，避免用 Pico W 去冲 8k。"
            : goals.find((item) => item.id === goal)?.hint}
        </p>
        <div className="-mx-4 overflow-x-auto px-4 sm:mx-0 sm:px-0">
          <Table>
            <TableHeader>
              <TableRow>
                <TableHead className="min-w-44">板子</TableHead>
                <TableHead>MCU</TableHead>
                <TableHead>USB</TableHead>
                <TableHead>有线</TableHead>
                <TableHead>蓝牙</TableHead>
                <TableHead>2.4G</TableHead>
                <TableHead className="min-w-56">备注</TableHead>
              </TableRow>
            </TableHeader>
            <TableBody>
              {visible.map((board) => (
                <TableRow key={board.name}>
                  <TableCell className="align-top">
                    <div className="flex flex-col gap-1">
                      <span className="font-medium text-foreground">
                        {board.name}
                      </span>
                      <div className="flex flex-wrap gap-1">
                        {board.pick ? (
                          <Badge>首选</Badge>
                        ) : null}
                        <Badge variant="secondary">{board.buy}</Badge>
                      </div>
                    </div>
                  </TableCell>
                  <TableCell className="align-top text-muted-foreground">
                    {board.mcu}
                  </TableCell>
                  <TableCell className="align-top">
                    <Badge
                      variant={board.usb === "HS" ? "default" : "outline"}
                    >
                      {board.usb === "HS" ? "High Speed" : "Full Speed"}
                    </Badge>
                  </TableCell>
                  <TableCell className="align-top text-muted-foreground">
                    {board.wiredHz}
                  </TableCell>
                  <TableCell className="align-top text-muted-foreground">
                    {board.btHz}
                  </TableCell>
                  <TableCell className="align-top text-muted-foreground">
                    {board.g24Hz}
                  </TableCell>
                  <TableCell className="align-top leading-6 text-muted-foreground">
                    {board.note}
                  </TableCell>
                </TableRow>
              ))}
            </TableBody>
          </Table>
        </div>
      </CardContent>
    </Card>
  );
}
