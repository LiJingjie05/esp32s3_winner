/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

// This demo UI is adapted from LVGL official example: https://docs.lvgl.io/master/widgets/extra/meter.html#simple-meter

#include "lvgl.h"

// 全局变量声明
static lv_obj_t *meter;       // 指针指向仪表对象
static lv_obj_t *btn;         // 按钮对象
static lv_disp_rot_t rotation = LV_DISP_ROT_NONE; // 屏幕旋转方向变量，初始为不旋转

// 设置仪表的数值（由动画控制）
static void set_value(void *indic, int32_t v)
{
    lv_meter_set_indicator_end_value(meter, indic, v);  // 设置指示器的结束值为 v
}

// 按钮点击回调函数
static void btn_cb(lv_event_t * e)
{
    lv_disp_t *disp = lv_event_get_user_data(e);  // 获取事件相关的显示设备对象
    rotation++;  // 增加旋转角度
    if (rotation > LV_DISP_ROT_270) {  // 如果旋转角度超过 270°
        rotation = LV_DISP_ROT_NONE;  // 重置旋转角度为无旋转
    }
    lv_disp_set_rotation(disp, rotation);  // 更新显示设备的旋转角度
}

// 主示例函数
void example_lvgl_demo_ui(lv_disp_t *disp)
{
    lv_obj_t *scr = lv_disp_get_scr_act(disp);  // 获取当前活动屏幕对象

    meter = lv_meter_create(scr);  // 创建一个仪表对象
    lv_obj_center(meter);  // 将仪表放置在屏幕中心
    lv_obj_set_size(meter, 200, 200);  // 设置仪表的大小为 200x200 像素

    /* 创建仪表刻度 */
    lv_meter_scale_t *scale = lv_meter_add_scale(meter);  // 向仪表添加一个刻度
    lv_meter_set_scale_ticks(meter, scale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY)); // 设置刻度，41 个刻度，每 2 个间隔一个大刻度
    lv_meter_set_scale_major_ticks(meter, scale, 8, 4, 15, lv_color_black(), 10);  // 设置主刻度线，8 个大刻度，每隔 4 个小刻度，长度为 15

    lv_meter_indicator_t *indic;

    /* 创建蓝色的弧线，表示开始部分 */
    indic = lv_meter_add_arc(meter, scale, 3, lv_palette_main(LV_PALETTE_BLUE), 0);  // 添加一个蓝色的弧形指示器
    lv_meter_set_indicator_start_value(meter, indic, 0);  // 设置指示器开始值为 0
    lv_meter_set_indicator_end_value(meter, indic, 20);  // 设置指示器结束值为 20

    /* 创建蓝色的刻度线，表示开始部分 */
    indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE), false, 0);  // 添加蓝色刻度线
    lv_meter_set_indicator_start_value(meter, indic, 0);  // 设置刻度线开始值为 0
    lv_meter_set_indicator_end_value(meter, indic, 20);  // 设置刻度线结束值为 20

    /* 创建红色的弧线，表示结束部分 */
    indic = lv_meter_add_arc(meter, scale, 3, lv_palette_main(LV_PALETTE_RED), 0);  // 添加一个红色的弧形指示器
    lv_meter_set_indicator_start_value(meter, indic, 80);  // 设置指示器开始值为 80
    lv_meter_set_indicator_end_value(meter, indic, 100);  // 设置指示器结束值为 100

    /* 创建红色的刻度线，表示结束部分 */
    indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false, 0);  // 添加红色刻度线
    lv_meter_set_indicator_start_value(meter, indic, 80);  // 设置刻度线开始值为 80
    lv_meter_set_indicator_end_value(meter, indic, 100);  // 设置刻度线结束值为 100

    /* 创建指针线指示器 */
    indic = lv_meter_add_needle_line(meter, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);  // 添加一个灰色的指针线指示器，长度为 4，偏移 -10

    // 创建一个按钮，用于旋转屏幕
    btn = lv_btn_create(scr);  // 创建一个按钮
    lv_obj_t *lbl = lv_label_create(btn);  // 在按钮上创建标签
    lv_label_set_text_static(lbl, LV_SYMBOL_REFRESH" ROTATE");  // 设置按钮的标签为 "旋转"
    lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 30, -30);  // 将按钮放置在屏幕的左下角

    // 为按钮添加点击事件回调
    lv_obj_add_event_cb(btn, btn_cb, LV_EVENT_CLICKED, disp);

    /* 创建一个动画，用于动态更新仪表的值 */
    lv_anim_t a;
    lv_anim_init(&a);  // 初始化动画
    lv_anim_set_exec_cb(&a, set_value);  // 设置动画的执行回调函数（即设置仪表值）
    lv_anim_set_var(&a, indic);  // 设置动画作用的对象（指针指示器）
    lv_anim_set_values(&a, 0, 100);  // 设置动画的起始值和结束值（0 到 100）
    lv_anim_set_time(&a, 500);  // 设置动画的时长为 2000 毫秒
    lv_anim_set_repeat_delay(&a, 100);  // 设置动画重复前的延时为 100 毫秒
    lv_anim_set_playback_time(&a, 500);  // 设置动画播放的反向时间为 500 毫秒
    lv_anim_set_playback_delay(&a, 100);  // 设置反向动画的延迟时间为 100 毫秒
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);  // 设置动画为无限次重复
    lv_anim_start(&a);  // 启动动画
}


void example_lvgl_demo_ui_test(lv_disp_t *disp)
{
    lv_obj_t *scr = lv_disp_get_scr_act(disp);
    lv_obj_set_style_bg_color(scr, lv_color_make(255, 255, 0), LV_PART_MAIN);

    lv_obj_t *btn1 = lv_btn_create(scr);
    lv_obj_set_pos(btn1, 0, 0);
    lv_obj_set_size(btn1, 80, 40);

    lv_obj_t *label = lv_label_create(btn1);
    lv_label_set_text(label, "button");

    
}
