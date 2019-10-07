# 吉里吉里Z用Steam向けDrawDeviceプラグイン

## 概要

吉里吉里Z向けのDrawDeviceプラグインです。
主に Steam 環境でフルスクリーンにてゲームを表示した際に
Steamオーバーレイの表示がゲーム範囲外の画面を汚すのを回避する目的で使用します。

吉里吉里2では使用できないので注意してください。
（※ソースコードには吉里吉里2向けのDrawDevice差異を埋めるラッパー用のコードの残骸がありますが，
現在は正しく動作していないためオミットされています）


## コンパイル方法

[Premake5](https://premake.github.io/) コマンドを使用してプロジェクト生成してコンパイルします。

```
premake5 vs2012
```
等。（vs2012の部分はお使いのコンパイラのバージョンに合わせて調整してください）


## 使い方

`Plugins.link("DrawDeviceForSteam.dll");` すると `DrawDeviceForSteam` がTJSから見えるようになりまので，
Window.drawDevice を差し替えてください。

サンプルとして Override_DrawDeviceHook.tjs を置いておきますので手持ちの Override.tjs に追記してください。


## ソースコードについて

`src/BasicDrawDevice.*` は吉里吉里Zの該当ソースコードを改造したものを使用しています。

`src/Messages.inc` は吉里吉里Zのリソース定義を改造したものを使用しています。

`tp_stubz/` フォルダ内のファイルは吉里吉里Z向けのプラグインインターフェースファイルです。

`simplebinder/` フォルダ内のファイルは [simplebinder](https://github.com/krkrz/krkr2/tree/master/kirikiri2/trunk/kirikiri2/src/plugins/win32/00_simplebinder) の派生ソースファイルです。


## ライセンス

吉里吉里Zのものに準拠してください


## 履歴

AUTHOR: miahmie

- 2019/10/06 初版
- 2019/10/08 フルスクリーン時の表示不具合を修正
