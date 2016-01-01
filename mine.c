#include <gtk/gtk.h>

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
};

/**
 * 全域變數
 */
static struct block *map; /* 棋盤區資料 */
static gint width = 8; /* 棋盤區寬度 */
static gint height = 4; /* 棋盤區高度 */

static gint white_chess = 16;   //黑色棋子數量
static gint black_chess = 16;   //白色棋子數量

static GtkWidget *white_chess_label; /* 顯示剩餘白色數量 */
static gint button_size = 80; /* button 大小 */

static gint opened_count; /* 已經掀開多少格子 */
static gboolean game_over; /* 遊戲是否已結束 */

static GtkWidget *window; /* 主視窗 */


/**
 * game start
 */
void game_init()
{

    /* 初始變數值 */
    opened_count = 0;
    game_over = FALSE;
    white_chess = 16;
    black_chess = 16;

    /* 顯示剩餘白色數量*/
    gchar buf[4];
    g_snprintf(buf, 4, "%d", white_chess);
    gtk_label_set_text(GTK_LABEL(white_chess_label), buf);

    /* 以亂數安置白色棋子*/
    gint size = width * height;
    gint i = 0;
    while (i < white_chess)
    {
        gint index;
        index = g_random_int_range(0, size);
        if (map[index].color == 1 || map[index].color == 2)
            continue;
        map[index].color = 1;

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

        i++;
    }

}

/**
 * gameover
 */
void gameover(gboolean won)
{
    GtkWidget *dialog;
    gchar msg[100];

    if (game_over == TRUE)
        return;

    game_over = TRUE;

}

/**
 * 翻開格子 (x,y)
 */
void open_block(gint x, gint y)
{
    gint index;
    GtkWidget *button;
    GtkWidget *image;
    index = x + y * width;
    button = map[index].button;
    
    
    /**
     * 改變 button 狀態為按下 .
     * TRUE 是按下, FALSE 則是未按下的狀態
     */
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

    if (map[index].opened == TRUE) /* 掀開的格子保持按下狀態即可 */
        return;

    map[index].opened = TRUE; /* 格子狀態為掀開 */

    if(map[index].color == 1)
    {
        image = gtk_image_new_from_file ("01.png");
        gtk_button_set_image(GTK_BUTTON(button), image);
    }
        
    else if(map[index].color == 2)
    {
        image = gtk_image_new_from_file ("02.png");
        gtk_button_set_image(GTK_BUTTON(button), image);
    }
        



    opened_count++; /* 已掀開的格子又多了一個 */

    /*
    if (opened_count + mines == width * height)
    {
        gameover(TRUE);  所有空地都被翻完時遊戲結束
        return;
    }*/


}
void on_start()
{
    g_print ("\n ************\n *  start!  *\n ************\n\n");
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
     * 檢查遊戲是否已經結束.
     * 若遊戲已結束, 玩家按下 button 也沒有反應
     */
    if (game_over == TRUE)
        return TRUE;

    /**
     * 將 callback 接收的 data 換成數字的 index 來使用.
     * 這個 index 也就是在 g_signal_connect() 中
     * 每個 button 自己的 index */
    index = (gint) data;

    switch (event->button)
    {
    case 1: /* 滑鼠左鍵 */
        /* 從 index 算出發生事件格子的行列 */
        row = index / width;
        col = index % width;
        /**
         * 掀開格子
         * (使用 open_block() 來掀開指定的格子)
         * */
        open_block(col, row);
        break;
    case 2: /* 滑鼠中鍵 */
        break;
    case 3: //滑鼠右鍵
            break;

        /* 顯示新的地雷數
        g_snprintf(buf, 4, "%d", MAX(0, mines - marked_count));
        gtk_label_set_text(GTK_LABEL(white_chess_label), buf);*/
    }

    /**
     * 傳回 TRUE 表示這個事件已經被處理完畢,
     * GTK 不需要再尋找其他 callback function 處理 */
    return TRUE;
}

/**
 * main
 */
int main(int argc, char **argv) {
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
	label = gtk_label_new("Mines:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 4);
	gtk_widget_show_all(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

	/* width x height 個 button 的區塊 */
	for (i = 0, index = 0; i < height; i++) {
		gint j;
		hbox = gtk_hbox_new(FALSE, 0);
		for (j = 0; j < width; j++) {
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