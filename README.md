# �g���g��Z�pSteam����DrawDevice�v���O�C��

## �T�v

�g���g��Z������DrawDevice�v���O�C���ł��B
��� Steam ���Ńt���X�N���[���ɂăQ�[����\�������ۂ�
Steam�I�[�o�[���C�̕\�����Q�[���͈͊O�̉�ʂ������̂��������ړI�Ŏg�p���܂��B

�g���g��2�ł͎g�p�ł��Ȃ��̂Œ��ӂ��Ă��������B
�i���\�[�X�R�[�h�ɂ͋g���g��2������DrawDevice���ق𖄂߂郉�b�p�[�p�̃R�[�h�̎c�[������܂����C
���݂͐��������삵�Ă��Ȃ����߃I�~�b�g����Ă��܂��j


## �R���p�C�����@

[Premake5](https://premake.github.io/) �R�}���h���g�p���ăv���W�F�N�g�������ăR���p�C�����܂��B

```
premake5 vs2012
```
���B�ivs2012�̕����͂��g���̃R���p�C���̃o�[�W�����ɍ��킹�Ē������Ă��������j


## �g����

`Plugins.link("DrawDeviceForSteam.dll");` ����� `DrawDeviceForSteam` ��TJS���猩����悤�ɂȂ�܂̂ŁC
Window.drawDevice �������ւ��Ă��������B

�T���v���Ƃ��� Override_DrawDeviceHook.tjs ��u���Ă����܂��̂Ŏ莝���� Override.tjs �ɒǋL���Ă��������B


## �\�[�X�R�[�h�ɂ���

`src/BasicDrawDevice.*` �͋g���g��Z�̊Y���\�[�X�R�[�h�������������̂��g�p���Ă��܂��B

`src/Messages.inc` �͋g���g��Z�̃��\�[�X��`�������������̂��g�p���Ă��܂��B

`tp_stubz/` �t�H���_���̃t�@�C���͋g���g��Z�����̃v���O�C���C���^�[�t�F�[�X�t�@�C���ł��B

`simplebinder/` �t�H���_���̃t�@�C���� [simplebinder](https://github.com/krkrz/krkr2/tree/master/kirikiri2/trunk/kirikiri2/src/plugins/win32/00_simplebinder) �̔h���\�[�X�t�@�C���ł��B


## ���C�Z���X

�g���g��Z�̂��̂ɏ������Ă�������


## ����

AUTHOR: miahmie

- 2019/10/06 ����
- 2019/10/08 �t���X�N���[�����̕\���s����C��
