#include <gtk/gtk.h>

/**
 * struct for 每個格子裡的資料
 */
struct block
{
    gint count; /* 周圍有多少地雷 */
    gboolean mine; /* 是否藏有地雷 */
    gboolean marked; /* 是否被標記過 */
    gboolean opened; /* 是否已被掀開 */
    GtkWidget *button;
};

/**
 * 全域變數
 */
static struct block *map; /* 地雷區資料 */
static gint width = 10; /* 地雷區寬度 */
static gint height = 10; /* 地雷區高度 */
static gint mines = 20; /* 地雷數量 */

static GtkWidget *mine_label; /* 顯示剩餘地雷數 */
static GtkWidget *time_label; /* 顯示遊戲時間 */
static gint button_size = 20; /* button 大小 */

static gint opened_count; /* 已經掀開多少格子 */
static gint marked_count; /* 已經標記多少格子 */
static gboolean game_over; /* 遊戲是否已結束 */

static GtkWidget *window; /* 主視窗 */
static gint game_time; /* 遊戲開始後的計秒 */

/**
 * g_timeout_add() ? callback function
 */
gboolean tick(gpointer data)
{
    gchar buf[8];

    if (game_over == TRUE) /* 遊戲結束則傳回 FALSE 停止計時 */
        return FALSE;

    game_time++; /* 增加遊戲時間並更新顯示 */
    g_snprintf(buf, 8, "%d", game_time);
    gtk_label_set_text(GTK_LABEL(time_label), buf);
    return TRUE; /* 傳回 TRUE 繼續計時 */
}

/**
 * game start
 */
void game_init()
{

    /* 初始變數值 */
    opened_count = 0;
    marked_count = 0;
    game_over = FALSE;
    game_time = 0;

    /* 顯示剩餘地雷數 */
    gchar buf[4];
    g_snprintf(buf, 4, "%d", MAX(0, mines - marked_count));
    gtk_label_set_text(GTK_LABEL(mine_label), buf);

    /* 以亂數安置地雷 */
    gint size = width * height;
    gint i = 0;
    while (i < mines)
    {
        gint index;
        gint row, col;
        index = g_random_int_range(0, size);
        if (map[index].mine == TRUE) /* 已有地雷 */
            continue;
        map[index].mine = TRUE;
        row = index / width;
        col = index % width;

        /* 四周格子的 count 加一 */
        if (row > 0)
        {
            /* 左上 */if (col > 0)
                map[index - width - 1].count++;
            /* 正上 */
            map[index - width].count++;
            /* 右上 */if (col < width - 1)
                map[index - width + 1].count++;
        }
        /* 左 */
        if (col > 0)
            map[index - 1].count++;
        /* 右 */
        if (col < width - 1)
            map[index + 1].count++;
        if (row < height - 1)
        {
            /* 左下 */if (col > 0)
                map[index + width - 1].count++;
            /* 正下 */
            map[index + width].count++;
            /* 右下 */if (col < width - 1)
                map[index + width + 1].count++;
        }

        i++;
    }

    /* 啟動計時 */
    g_timeout_add(1000, (GSourceFunc) tick, NULL);
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

    if (won == TRUE)   /* 準備掃雷成功的訊息 */
    {
        g_snprintf(msg, 100, "You won! You have cleared"
                   " the game in %3d seconds.", game_time);
    }
    else     /* 準備踩到地雷的訊息 */
    {
        // === show mine ===
        int i, n;
        n = width * height;
        for (i = 0; i < n; i++)
        {
            if (map[i].mine == TRUE)
            {
                gtk_button_set_label(GTK_BUTTON(map[i].button), "*");
            }
        }
        // === show mine ===
        g_snprintf(msg, 100, "Bad luck. Game over.");
    }

    /* 用 GtkMessageDialog 顯示訊息 */
    dialog = gtk_message_dialog_new(GTK_WINDOW(window), 0, GTK_MESSAGE_INFO,
                                    GTK_BUTTONS_OK, msg);
    gtk_dialog_run(GTK_DIALOG(dialog));
    //game_init();
    gtk_widget_destroy(dialog);
}

/**
 * 翻開格子 (x,y)
 */
void open_block(gint x, gint y)
{
    gint index;
    GtkWidget *button;

    index = x + y * width;

    /* 遊戲已結束或防止玩家誤翻有記號的格子 */
    if (map[index].marked == TRUE)
        return;

    button = map[index].button;

    /**
     * 改變 button 狀態為按下 .
     * TRUE 是按下, FALSE 則是未按下的狀態
     */
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

    if (map[index].opened == TRUE) /* 掀開的格子保持按下狀態即可 */
        return;

    map[index].opened = TRUE; /* 格子狀態為掀開 */

    if (map[index].mine == TRUE)  /* 若藏有地雷 */
    {
        gtk_button_set_label(GTK_BUTTON(button), "*");
        gameover(FALSE);/* 踩到地雷遊戲結束 */
        return;
    }

    if (map[index].count > 0)   /* 若周圍有地雷 */
    {
        gchar buf[2];
        g_snprintf(buf, 2, "%d", map[index].count);
        gtk_button_set_label(GTK_BUTTON(button), buf);
    }

    opened_count++; /* 已掀開的格子又多了一個 */

    if (opened_count + mines == width * height)
    {
        gameover(TRUE); /* 所有空地都被翻完時遊戲結束 */
        return;
    }

    if (map[index].count == 0)  /* 若周圍沒有地雷 */
    {
        /* 掀開周圍格子 */
        if (y > 0)
        {
            if (x > 0)
                open_block(x - 1, y - 1);
            open_block(x, y - 1);
            if (x < width - 1)
                open_block(x + 1, y - 1);
        }
        if (x > 0)
            open_block(x - 1, y);
        if (x < width - 1)
            open_block(x + 1, y);
        if (y < height - 1)
        {
            if (x > 0)
                open_block(x - 1, y + 1);
            open_block(x, y + 1);
            if (x < width - 1)
                open_block(x + 1, y + 1);
        }
    }
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
    case 3: /* 滑鼠右鍵 */
        /* 已掀開的格子不做記號 */
        if (map[index].opened == TRUE)
            break;
        /* 原來有記號則消掉, 沒有則畫上記號 */
        if (map[index].marked != TRUE)
        {
            map[index].marked = TRUE;
            gtk_button_set_label(GTK_BUTTON(widget), "@");
            marked_count++;
        }
        else
        {
            map[index].marked = FALSE;
            gtk_button_set_label(GTK_BUTTON(widget), "");
            marked_count--;
        }
        /* 顯示新的地雷數 */
        g_snprintf(buf, 4, "%d", MAX(0, mines - marked_count));
        gtk_label_set_text(GTK_LABEL(mine_label), buf);
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
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *label;
    gint i, j, index;

    /* initialize GTK library */
    gtk_init(&argc, &argv);

    /* 分配記憶體給 map 並初始化 */
    map = (struct block *) g_malloc0(sizeof(struct block) * width * height);

    /* build the user interface */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window), "delete_event", gtk_main_quit, NULL);

    vbox = gtk_vbox_new(FALSE, 0);

    /* 存放 label 的第一個 hbox */
    hbox = gtk_hbox_new(FALSE, 0);
    label = gtk_label_new("Mines:");
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 4);
    mine_label = gtk_label_new("0");
    gtk_box_pack_start(GTK_BOX(hbox), mine_label, FALSE, FALSE, 2);
    label = gtk_label_new("Time:");
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 4);
    time_label = gtk_label_new("0");
    gtk_box_pack_start(GTK_BOX(hbox), time_label, FALSE, FALSE, 2);
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

    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show(vbox);
    gtk_widget_show(window);

    game_init();

    gtk_main();

    g_free(map); /* 釋放記憶體 */

    return 0;
}

