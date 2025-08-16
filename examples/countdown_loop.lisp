;; ループ風カウントダウンタイマー
;; より効率的な実装を試みる

(print "==============================")
(print "  ループ風カウントダウンタイマー")
(print "==============================")
(print "")

;; カウントダウン関数を定義（疑似ループ）
;; 現在のLISP処理系の制限により、手動展開でループを模擬

(print "カウントダウン開始...")
(print "")

;; 10秒バージョン（短縮版でテスト）
(print "10") (sleep 1)
(print "9")  (sleep 1)
(print "8")  (sleep 1)
(print "7")  (sleep 1)
(print "6")  (sleep 1)
(print "5")  (sleep 1)
(print "4")  (sleep 1)
(print "3")  (sleep 1)
(print "2")  (sleep 1)
(print "1")  (sleep 1)

(print "")
(print "*** 時間終了！ ***")
(print "")

;; 条件分岐のテスト
(print "条件分岐テスト:")
(print (= 1 1))
(print (= 1 2))
