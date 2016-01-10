#include <gtk/gtk.h>
#include <unistd.h>
#include <stdlib.h>
/**
 * struct for 每個格子裡的資料
 */
GtkWidget *vbox;
GtkWidget *hbox;
GtkWidget *label;
GtkWidget *title;
GtkWidget *start;
GtkWidget *quit;
GtkWidget *table;
struct block
{
    gint color;     //紅色1 黑色2
    gint weight;    //代表棋子大小
    GtkWidget *image;   //儲存棋子的圖
    GtkWidget *button;
    gboolean opened; /* 是否已被掀開 */
    gboolean picked_up; //是否被pick up
};
static GtkWidget *label_1; //////////test///////
static GtkWidget *label_2; //////////test///////
/**
 * 全域變數
 */
static struct block *map; /* 棋盤區資料 */
static gint width = 8; /* 棋盤區寬度 */
static gint height = 4; /* 棋盤區高度 */

static gint red_chess = 16;   //紅色棋子數量
static gint black_chess = 16;   //黑色棋子數量


static gint turn;   //表示現在是誰的回合 1為先手 2為後手
static gboolean pick_up;     //是否pick up棋子
static gint picked_up_index; //picked up 的棋子的index

static GtkWidget *red_chess_label; // 顯示剩餘紅色數量
static GtkWidget *black_chess_label; // 顯示剩餘黑色數量

static gint button_size = 80; /* button 大小 */

static gint opened_count; /* 已經掀開多少格子 */
static gboolean game_over; /* 遊戲是否已結束 */

static GtkWidget *window; /* 主視窗 */

gchar buf[8];
/**
 * game start
 */
void game_init()
{

    /* 初始變數值 */
    opened_count = 0;
    game_over = FALSE;
    red_chess = 16;
    black_chess = 16;

    /* 顯示剩餘紅色數量*/
    gchar buf[4];
    g_snprintf(buf, 4, "%d", red_chess);
    gtk_label_set_text(GTK_LABEL(red_chess_label), buf);

    /* 以亂數安置紅色棋子*/
    gint size = width * height;
    gint i = 0;
    while (i < red_chess)
    {
        gint index;
        index = g_random_int_range(0, size);
        if (map[index].color == 1 || map[index].color == 2)
            continue;
        map[index].color = 1;

        //寫入權重以及圖片
        switch(i)
        {
        case 0:
            map[index].weight = 1;  //帥的權重
            map[index].image = gtk_image_new_from_file ("img/01-01.png");   //帥的圖片
            break;
        case 1:
        case 2:
            map[index].weight = 2;  //仕的權重
            map[index].image = gtk_image_new_from_file ("img/01-02.png");   //仕的圖片
            break;
        case 3:
        case 4:
            map[index].weight = 3;  //相的權重
            map[index].image = gtk_image_new_from_file ("img/01-03.png");   //相的圖片
            break;
        case 5:
        case 6:
            map[index].weight = 4;  //俥的權重
            map[index].image = gtk_image_new_from_file ("img/01-04.png");   //俥的圖片
            break;
        case 7:
        case 8:
            map[index].weight = 5;  //傌的權重
            map[index].image = gtk_image_new_from_file ("img/01-05.png");   //傌的圖片
            break;
        case 9:
        case 10:
            map[index].weight = 6;  //炮的權重
            map[index].image = gtk_image_new_from_file ("img/01-06.png");   //炮的圖片
            break;
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            map[index].weight = 7;  //兵的權重
            map[index].image = gtk_image_new_from_file ("img/01-07.png");   //兵的圖片
            break;
        }

        i++;
    }
    /* 以亂數安置黑色棋子*/
    i = 0;
    while (i < black_chess)
    {
        gint index;
        index = g_random_int_range(0, size);
        if (map[index].color == 1 || map[index].color == 2)
            continue;
        map[index].color = 2;

        //寫入權重
        switch(i)
        {
        case 0:
            map[index].weight = 1;  //將的權重
            map[index].image = gtk_image_new_from_file ("img/02-01.png");   //將的圖片
            break;
        case 1:
        case 2:
            map[index].weight = 2;  //士的權重
            map[index].image = gtk_image_new_from_file ("img/02-02.png");   //士的圖片
            break;
        case 3:
        case 4:
            map[index].weight = 3;  //象的權重
            map[index].image = gtk_image_new_from_file ("img/02-03.png");   //象的圖片
            break;
        case 5:
        case 6:
            map[index].weight = 4;  //車的權重
            map[index].image = gtk_image_new_from_file ("img/02-04.png");   //車的圖片
            break;
        case 7:
        case 8:
            map[index].weight = 5;  //馬的權重
            map[index].image = gtk_image_new_from_file ("img/02-05.png");   //馬的圖片
            break;
        case 9:
        case 10:
            map[index].weight = 6;  //包的權重
            map[index].image = gtk_image_new_from_file ("img/02-06.png");   //包的圖片
            break;
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            map[index].weight = 7;  //卒的權重
            map[index].image = gtk_image_new_from_file ("img/02-07.png");   //卒的圖片
            break;
        }

        i++;
    }

}


/**
 * 翻開格子 (x,y)
 */
void open_or_pick(gint x, gint y)
{
    gint index;
    GtkWidget *button;
    GtkWidget *image;
    index = x + y * width;
    button = map[index].button;
    image =  map[index].image;


    //選擇明棋 並移動
    if(map[index].opened == TRUE || pick_up == TRUE)
    {
        //限定一次只能pick up一個棋子
        if(!pick_up)
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), FALSE);

        pick_up = TRUE;
        map[index].picked_up = TRUE;
        picked_up_index = index;


    }
    //選擇暗棋 翻開
    else
    {
        //棋子狀態為掀開 設定棋子圖片
        map[index].opened = TRUE;
        gtk_button_set_image(GTK_BUTTON(button), image);
        //設定按鈕為壓下狀態
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    }



    opened_count++; /* 已掀開的格子又多了一個 */

    /*
    if (opened_count + mines == width * height)
    {
        gameover(TRUE);  所有空地都被翻完時遊戲結束
        return;
    }*/


}

void eat(int index)
{
    GtkWidget *image = map[index].image;
    GtkWidget *picked_up_image = map[picked_up_index].image;
    GtkWidget *blank_image = gtk_image_new();


    gtk_button_set_image(GTK_BUTTON(map[index].button), blank_image);
    gtk_button_set_image(GTK_BUTTON(map[index].button), picked_up_image);

    if(map[index].color==1)
        red_chess--;
    else if(map[index].color==2)
        black_chess--;

    //將picked up的棋子的資料寫到被吃掉的棋子的按鈕 除了button
    map[index].color = map[picked_up_index].color;
    map[index].weight = map[picked_up_index].weight;
    map[index].image = map[picked_up_index].image;
    //map[index].button = gtk_toggle_button_new();
    map[index].opened = map[picked_up_index].opened;
    map[index].picked_up = map[picked_up_index].picked_up;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(map[index].button), FALSE);



    //設定原本picked up位置的按鈕
    map[picked_up_index].color = 0;
    map[picked_up_index].weight = 8;    //代表空白 任何棋子都可移動到上面
    map[picked_up_index].image = NULL;
    //map[picked_up_index].button = gtk_toggle_button_new();
    map[picked_up_index].opened = TRUE;
    map[picked_up_index].picked_up = FALSE;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(map[picked_up_index].button), TRUE);

    //設定新的picked up index
    picked_up_index = index;

}

void red_wins()
{
    sleep(1);
    GtkWidget *window2,*label2;
    gtk_widget_hide (vbox);
    window2 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window2), "Red Wins!");
    g_signal_connect(GTK_OBJECT(window2), "destroy",G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show(window2);
    label2=gtk_label_new("Red Wins!");
    gtk_container_add (GTK_CONTAINER (window2),label2);
    gtk_widget_show(label2);
    gtk_main();
}
void black_wins()
{
    sleep(1);
    GtkWidget *window2,*label2;
    gtk_widget_hide (vbox);
    window2 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window2), "Black Wins!");
    g_signal_connect(GTK_OBJECT(window2), "destroy",G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show(window2);
    label2=gtk_label_new("Black Wins!");
    gtk_container_add (GTK_CONTAINER (window2),label2);
    gtk_widget_show(label2);
    gtk_main();
}

void move_chess(gint x, gint y)
{
    gint index;
    GtkWidget *button;
    GtkWidget *image;
    GtkWidget *picked_up_image;
    index = x + y * width;
    button = map[index].button;
    image =  map[index].image;
    picked_up_image = map[picked_up_index].image;
    int chess_count = 0;    //計算兩個棋子之間有多少個棋子
    
    
    //如果選擇pick up的棋子 將棋子放下 pick_up設為FASLE
    if(map[index].picked_up)
    {
        map[index].picked_up = FALSE;
        pick_up = FALSE;
        picked_up_index = -1;
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    }
    else
    {
        //針對"炮" "包" 做特別處理
        if(map[picked_up_index].weight == 6 && (map[index].color != map[picked_up_index].color))
        {

            //計算到目標位置間的image數 假如只有一個代表與目標間只有一個棋子
            //同列
            if(index / 8 == picked_up_index / 8)
            {
                int tmp;

                if(index < picked_up_index)
                {
                    tmp = index+1;
                    for(; tmp < picked_up_index; ++tmp)
                    {
                        if(map[tmp].weight != 8)
                            ++chess_count;
                    }
                }
                else if(index > picked_up_index)
                {
                    tmp = picked_up_index+1;
                    for(; tmp < index; ++tmp)
                    {
                        if(map[tmp].weight != 8)
                            ++chess_count;
                    }
                }



            }
            //同行
            else if(index % 8 == picked_up_index % 8)
            {
                int tmp;

                if(index < picked_up_index)
                {
                    tmp = index+8;
                    for(; tmp < picked_up_index; tmp+=8)
                    {
                        if(map[tmp].weight != 8)
                            ++chess_count;
                    }
                }
                else if(index > picked_up_index)
                {
                    tmp = picked_up_index+8;
                    for(; tmp < index; tmp+=8)
                    {
                       if(map[tmp].weight != 8)
                            ++chess_count;
                    }
                }


            }
            
                

            if(chess_count == 1)
                eat(index);
            chess_count = 0;
        }



        //限定只能往↑↓←→移動
        else if(index == picked_up_index -1 || index == picked_up_index +1 ||
                index == picked_up_index +8 || index == picked_up_index -8)
        {
            //翻開移動方向的棋子
            if(!map[index].opened)  //第一次翻開時才須設定圖片
            {
                gtk_button_set_image(GTK_BUTTON(button), image);
                map[index].opened = TRUE;
            }

            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);



            //判斷是否能吃對方
            if(map[index].color != map[picked_up_index].color)
            {
                //針對"帥" "兵" "將" "卒" 做特別處理
                if( (map[index].weight == 7 && map[picked_up_index].weight == 1) ||
                        (map[index].weight == 1 && map[picked_up_index].weight == 7) )
                {
                    if(map[index].weight == 7 && map[picked_up_index].weight == 1)
                        return;

                    eat(index);
                }
                else if(map[index].weight >= map[picked_up_index].weight)
                {
                    eat(index);
                }



            }
            //else //不能吃



        }

    }


    ///////////test//////////////
    g_snprintf(buf, 4, "%d", chess_count);
    gtk_label_set_text(GTK_LABEL(label_1), buf);
    g_snprintf(buf, 4, "%d", chess_count);
    gtk_label_set_text(GTK_LABEL(label_2), buf);

    printf("red %d black %d\n",red_chess,black_chess);
    if(black_chess==0)
        red_wins();
    else if(red_chess==0)
        black_wins();
}




void on_start()
{
    gtk_widget_hide (table);
    gtk_widget_hide (start);
    gtk_widget_hide (quit);
    gtk_widget_hide (title);
    gtk_container_remove(GTK_CONTAINER(window), table);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show(vbox);

}

/**
 * on_mouse_click callback function
 */
int on_mouse_click(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gint index;
    gint row, col;
    gchar buf[4];


    /**
     * 將 callback 接收的 data 換成數字的 index 來使用.
     * 這個 index 也就是在 g_signal_connect() 中
     * 每個 button 自己的 index */
    index = (gint) data;

    switch (event->button)
    {
    case 1: //滑鼠左鍵
        //從 index 算出發生事件格子的行列
        row = index / width;
        col = index % width;

        //如果有pick up棋子 執行move_chess 反之執行open_or_pick
        if(pick_up)
            move_chess(col, row);
        else
            open_or_pick(col, row);
        break;
    case 2: //滑鼠中鍵
        break;
    case 3: //滑鼠右鍵
        break;

        /* 顯示新的地雷數
        g_snprintf(buf, 4, "%d", MAX(0, mines - marked_count));
        gtk_label_set_text(GTK_LABEL(red_chess_label), buf);*/
    }

    /**
     * 傳回 TRUE 表示這個事件已經被處理完畢,
     * GTK 不需要再尋找其他 callback function 處理 */
    return TRUE;
}

/**
 * main
 */
int main(int argc, char **argv)
{
    gint i, j, index;


    /* initialize GTK library */
    gtk_init(&argc, &argv);

    /* 分配記憶體給 map 並初始化 */
    map = (struct block *) g_malloc0(sizeof(struct block) * width * height);

    /* build the user interface */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window), "delete_event", gtk_main_quit, NULL);
    gtk_window_set_default_size(GTK_WINDOW(window),640,350);


    /*作出標題畫面 */
    table = gtk_table_new (5, 5, TRUE);
    gtk_container_add (GTK_CONTAINER (window), table);
    gtk_widget_show (table);

    title = gtk_label_new("test");
    gtk_table_attach_defaults (GTK_TABLE (table), title, 1, 4, 0, 2);
    gtk_widget_show (title);

    start = gtk_button_new_with_label ("START");
    g_signal_connect (G_OBJECT (start), "clicked",G_CALLBACK (on_start), (gpointer) "start_button");
    gtk_table_attach_defaults (GTK_TABLE (table), start, 1, 4, 2, 3);
    gtk_widget_show (start);

    quit = gtk_button_new_with_label ("Quit");
    g_signal_connect (G_OBJECT (quit), "clicked",G_CALLBACK (gtk_main_quit), NULL);
    gtk_table_attach_defaults (GTK_TABLE (table), quit, 1, 4, 3, 4);
    gtk_widget_show (quit);
    /* 作出標題畫面 */

    vbox = gtk_vbox_new(FALSE, 0);

    /* 存放 label 的第一個 hbox */
    hbox = gtk_hbox_new(FALSE, 0);

    ////////////////test/////////
    label_1 = gtk_label_new("0");
    gtk_box_pack_start(GTK_BOX(hbox), label_1, FALSE, FALSE, 4);
    label_2 = gtk_label_new("0");//////test////////
    gtk_box_pack_start(GTK_BOX(hbox), label_2, FALSE, FALSE, 2);//test//



    gtk_widget_show_all(hbox);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    /* width x height 個 button 的區塊 */
    for (i = 0, index = 0; i < height; i++)
    {
        gint j;
        hbox = gtk_hbox_new(FALSE, 0);
        for (j = 0; j < width; j++)
        {
            GtkWidget *button;
            button = gtk_toggle_button_new();
            gtk_widget_set_usize(button, button_size, button_size);
            /**
             * 設定 button 的屬性使它不會成為輸入的 focus.
             * (否則有一個 button 上會有框, 很難看)
             */
            g_object_set(G_OBJECT(button), "can-focus", FALSE, NULL);
            gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
            gtk_widget_show(button);

            g_signal_connect(G_OBJECT(button), "button-press-event",
                             G_CALLBACK(on_mouse_click), (gpointer) index);
            map[index].button = button;
            index++;
        }
        gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
        gtk_widget_show(hbox);
    }

    gtk_widget_show(window);

    game_init();

    gtk_main();

    g_free(map); /* 釋放記憶體 */

    return 0;
}
