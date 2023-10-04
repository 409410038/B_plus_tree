## **B+ Tree實作**


#### 作者訊息：國立中正大學資訊工程學系四年級 王郁淳

#### 聯絡資訊：u09410038@alum.ccu.edu.tw

#### 編譯器版本資訊
    使用C++，Code::Blocks 20.03撰寫
![](https://hackmd.io/_uploads/ry8lcgolT.png)

#### 如何操作此程式
1. 使用可編譯執行C++的環境執行，例如：Code::Blocks、Visual Studio Code，有在Visual Studio Code demo的詳細說明影片，附在繳交的作業中，若影片無法正常播放，可以到我上傳的雲端播放，連結為：https://drive.google.com/drive/folders/1N9tGxxbXfnpN3KV9_c-x6S6OiSsYXUrU?usp=sharing

2. 一開始需要先輸入B+ tree的order，order須為正整數，若輸入負數會報錯，若想結束程式可以直接輸入Quit(或quit)。
 
    ![](https://hackmd.io/_uploads/ry7Fqxslp.png)

3. 輸入完order之後會印出簡單的manual，共5種Command可以使用，每次輸入一種Command後須以Enter結尾，例如：Insert 8 10\n，第一個字母大寫或是小寫都可以，例如：Insert/ insert、Delete/delete…。

    ![](https://hackmd.io/_uploads/SJ695ljlp.png)

4. 各指令使用說明：
    * Insert/Delete後面可以接著一或一個以上的整數 (雖然題目敘述是正整數，但我稍微試過負整數的資料也可以正常處理)，每個數字處理完都會印出結果，例如：insert 10 15，會先印出insert 10的結果，接著印出insert 15的結果，以此類推。
    * Find 指令後面需要接提供的四種operator其中一種，然後再接一個整數
    * Restart/Quit則如manual的用法。

    **一行指令間每個token須以空格隔開，結尾須為\n**
 
 
 #### 程式運作說明
1.  Redistribution Insert: 
    
    遇到目標node空間不足時，在做一般的splitting insert之前，會先嘗試做redistribution來減少增長樹高的次數。
    
    做法的主要概念為：先把要Insert的數字先加入原本的leaf node (以下稱為current node) ，使它overflow之後，檢查current node的left node或right node是否有空間，若是左邊有空間，則將current node的最小值移到left node，並找到它們最近共同parent (或ancenstor) 來更新index，若是右邊有空間，則將current node的最大值移到right node，然後更新index。
    
    舉例來說，目前的樹長這樣：
    
    ![](https://hackmd.io/_uploads/S1g0oejea.png)

    接著insert 31的時候，current node為[30,51]，因為left node[29,_]有空間，所以把30(current node overflow之後的最小值)加入left node，並把這兩個leaf node的共同最近的parent(或ancenstor)[30:_]，更新為[31:_]。
    
    下圖為insert 31的結果
    
    ![](https://hackmd.io/_uploads/BkQM3gilp.png)

    此程式中所有的insert都會先嘗試做redistribution，若失敗才會做page splitting
    
2. Delete Command: 

    若Delete該data之後會造成underflow，則處理underflow的步驟為：先嘗試做redistribution，若失敗則做和sibling的merge。
    
    redistribution的例子，假如目前的樹長這樣
    
    ![](https://hackmd.io/_uploads/r1pShxol6.png)

    delete 51時，current node從[51,_]變成[_,_]，造成underflow，則從left node或right node找，right node少一個值依舊滿足half capacity，因此將right node的最小值73移到current node，並更新parent的index entry。

    結果如下圖
    
    ![](https://hackmd.io/_uploads/ByMw2lsla.png)

    而merge的例子，假如目前的樹長這樣
    
    ![](https://hackmd.io/_uploads/SyrpneogT.png)


    delete 30的時候，因為左右都沒有可以移動的值，因此將current node[_,_] ([30,_]刪掉30造成underflow)，和sibling(同parent的left/right node)[31,_]做合併，變成[31,_]，並更新parent的index entry為(_,_)，因為parent變成underflow，所以再接著對parent做underflow的處理，一直往上檢查到root為止。下圖為delete 30的結果
    
    ![](https://hackmd.io/_uploads/HktA2gilp.png)

3. Find Command: 
    
    因為leaf node間為doubly linked list，所以可以順便做簡單的範圍搜尋(輸出沒有做sort)，舉例來說，輸入Find >= 30，則可以得到下圖的結果
    
    ![](https://hackmd.io/_uploads/ryagTgiea.png)


    可以使用的運算元為>, >=, <, <=，輸入格式為Find {operator} {integer}
    
4. 遇到相同data的處理：
    
    
    假如insert的值已經存在於樹中，則會忽略該值，繼續執行其他動作，舉例來說，30已經在樹中，再次insert 30時，結果如下圖
    
    ![](https://hackmd.io/_uploads/ByVEagjep.png)


    而若是delete不存在樹中的值，也會忽略該值，例如：
    
    
    ![](https://hackmd.io/_uploads/HJzralola.png)


5. Error input tolerance: 
    
    當輸入打錯，或是非設定好功能的關鍵詞，則會印出提醒，重新輸入想要的指令即可，下圖做個範例，
    
    ![](https://hackmd.io/_uploads/BkMdTese6.png)


6. Restart Command: 
    
    因為有時候要使用多組測資，就需要重新執行程式，因此我做了restart的功能，輸入Restart後，程式會再從輸入order的步驟開始。
    
    實作的作法為把原本的樹從root開始，recursive的把children node都delete掉，最後root也delete後，把root的指標設為nullptr，重新建樹時再去construct一個新的root出來，避免memory leak
    
    ![](https://hackmd.io/_uploads/HyZ5pxolp.png)
