;; 30秒カウントダウンタイマー (dotimes版)
;; Common Lisp標準のdotimesを使用

(print "==============================")
(print "    30秒カウントダウンタイマー")
(print "    (dotimes版)")
(print "==============================")
(print "")

(print "カウントダウン開始...")
(print "")

;; dotimesでカウントダウン：30から1へ
;; (dotimes (var count) body...)の形式
(dotimes (i 30)
  (print (- 30 i))  ; 30から開始して減らしていく
  (sleep 1))

(print "")
(print "*** 時間終了！ ***")
(print "")
(print "==============================")
(print "    タイマー完了")
(print "==============================")
