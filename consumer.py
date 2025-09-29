#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
consumer_gui.py  ——  consumer_helper.exe 的持久前端
指令集：load / save / query / insert / delete
"""
import subprocess, json, os, re, tkinter as tk
from tkinter import ttk, filedialog, messagebox

# ---------- 降半音键盘映射（传统钢琴） ----------
LOWER_MAP = {1: 7, 2: 2, 3: 3, 4: 3, 5: 5, 6: 6, 7: 7}   # 降4=3，降1=7（仅高组）
UNIT = 6400
DUR_MAP = {"三十二分": UNIT // 8, "十六分": UNIT // 4, "八分": UNIT // 2,
           "四分": UNIT, "二分": 2 * UNIT, "全分": 4 * UNIT}

# ---------- 后端驱动 ----------
class Backend:
    def __init__(self, exe):
        self.proc = subprocess.Popen([exe], stdin=subprocess.PIPE,
                                     stdout=subprocess.PIPE,
                                     stderr=subprocess.PIPE,
                                     text=True, bufsize=1,
                                     creationflags=subprocess.CREATE_NO_WINDOW)
    def _call(self, cmd: str):
        self.proc.stdin.write(cmd + '\n')
        self.proc.stdin.flush()
        return json.loads(self.proc.stdout.readline())

    def load(self, path):      return self._call(f'load {path} 0')
    def save(self, path):      return self._call(f'save {path}')
    def query(self):           return self._call('query')
    def insert(self, bid, t, cmd): return self._call(f'insert {bid} {t} {cmd}')
    def delete(self, lid):     return self._call(f'delete {lid}')
    def quit(self):            self._call('quit'); self.proc.wait()

# ---------- GUI ----------
class GUI:
    def __init__(self, root, backend):
        self.backend = backend
        self.undo_stack = []          # [(op, data), ...]
        root.title("Warframe 三弦琴肖申克作曲机")
        root.geometry("500x600")

        # 顶部工具条
        bar = ttk.Frame(root)
        bar.pack(fill='x', padx=5, pady=5)
        ttk.Button(bar, text="加载", command=self.on_load).pack(side='left', padx=2)
        ttk.Button(bar, text="保存", command=self.on_save).pack(side='left', padx=2)
        ttk.Button(bar, text="尾部插入", command=self.on_append).pack(side='left', padx=2)
        ttk.Button(bar, text="撤销", command=self.on_undo).pack(side='left', padx=2)
        ttk.Button(bar, text="前插", command=self.on_insert_before).pack(side='left', padx=2)

        # 音符+时值输入区
        inp = ttk.LabelFrame(root, text="插入/追加参数")
        inp.pack(fill='x', padx=5, pady=5)
        ttk.Label(inp, text="音符:").grid(row=0, column=0, sticky='e')
        self.note_var = tk.StringVar(value='1')
        ttk.Combobox(inp, textvariable=self.note_var,
                     values=list('1234567'), width=5).grid(row=0, column=1)
        ttk.Label(inp, text="高组").grid(row=0, column=2, sticky='e')
        self.high_var = tk.BooleanVar()
        ttk.Checkbutton(inp, variable=self.high_var).grid(row=0, column=3)
        ttk.Label(inp, text="降半音").grid(row=0, column=4, sticky='e')
        self.lower_var = tk.BooleanVar()
        ttk.Checkbutton(inp, variable=self.lower_var).grid(row=0, column=5)
        
        # 时值复选框（可多选组合）
        ttk.Label(inp, text="时值:").grid(row=1, column=0, sticky='e')
        self.dur_vars = {k: tk.BooleanVar(value=(k == "四分")) for k in DUR_MAP}
        for idx, name in enumerate(self.dur_vars):
            ttk.Checkbutton(inp, text=name, variable=self.dur_vars[name]).grid(row=1, column=idx + 1)

        # 序列预览
        ttk.Label(root, text="序列预览（双击删除）").pack(anchor='w', padx=5)
        self.listbox = tk.Listbox(root, height=15)
        self.listbox.pack(fill='both', expand=True, padx=5, pady=5)
        self.listbox.bind('<Double-Button-1>', self.on_delete_selected)

    # ---------- 功能 ----------
    def on_load(self):
        path = filedialog.askopenfilename(filetypes=[("TXT", "*.txt")])
        if not path: return
        self.backend.load(path)
        self.refresh()

    def build_cmd(self,note: int, high: bool, lower: bool) -> str:
        """
        note  : 1~7
        high  : 是否升组
        lower : 是否降调
        返回  : "1" | "1#" | "1~" | "1#~" | "高1" | "高1#" ...
        """
        suffix = ''
        if lower: suffix += '#'
        if high:  suffix += '~'
        # 高组前缀 + 数字 + 后缀（顺序任意，但只拼一次）
        base = str(note)
        return base + suffix
    
    def on_save(self):
        path = filedialog.asksaveasfilename(defaultextension=".txt", filetypes=[("TXT", "*.txt")])
        if not path: return
        self.backend.save(path)
        messagebox.showinfo("完成", "已保存")

    def on_append(self):
        self.insert_at(99999999)          # before_id=0 表示尾部

    def phys_to_logic(self, phys_idx):
        """直接取隐藏列表，O(1)"""
        return self.hidden_logic[phys_idx]
    
    def insert_at(self, before_id):
        # 组合时值
        total = sum(DUR_MAP[k] for k, v in self.dur_vars.items() if v.get())
        if total == 0:
            messagebox.showwarning("时值", "至少勾选一个时值"); return
        # 降半音映射
        cmd = self.build_cmd(int(self.note_var.get()),
                self.high_var.get(),
                self.lower_var.get())
        if not re.fullmatch(r'[1-7][#~]*', cmd):
            messagebox.showwarning("格式", "音符只能 1~7 后跟 # 或 ~"); return
        for v in self.dur_vars.values():
            v.set(False)
        # 默认勾回「四分」方便下次
        self.dur_vars["四分"].set(True)
        # 真正插入
        self.backend.insert(before_id, total, cmd)
        self.undo_stack.append(('ins', before_id, total, cmd))
        self.refresh()

    def on_undo(self):
        if not self.undo_stack:
            messagebox.showinfo("提示", "无操作可撤销"); return
        op, *args = self.undo_stack.pop()
        if op == 'ins':
            bid, t, cmd = args
            self.backend.delete(bid)          # 把刚插的删掉
        elif op == 'del':
            lid, = args
            # 重新插回被删的那一行（时间用 6400 占位，用户可再编辑）
            self.backend.insert(lid - 1, UNIT, "7")  # 用任意音符占位，用户可再改
        self.refresh()

    def refresh(self):
        self.listbox.delete(0, tk.END)
        for item in self.backend.query():
            self.listbox.insert(tk.END, f"{item['t']} {item['cmd']}")
        self.hidden_logic = [it['logicId'] for it in self.backend.query()]
    def on_insert_before(self):
        sel = self.listbox.curselection()
        if not sel: return
        before_id = self.phys_to_logic(sel[0])
        self.insert_at(before_id)

    def on_delete_selected(self, _=None):
        sel = self.listbox.curselection()
        if not sel: return
        logic_id = self.phys_to_logic(sel[0])
        self.backend.delete(logic_id)
        self.undo_stack.append(('del', logic_id))
        self.refresh()

# ---------- 启动 ----------
if __name__ == "__main__":
    root = tk.Tk()
    backend = Backend("./consumer_helper2.exe")   # 同目录
    GUI(root, backend)
    root.mainloop()
    backend.quit()
