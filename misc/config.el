
; s. http://archive.oreilly.com/pub/h/2044

;; compilation-shell-minor-mode
;; https://github.com/filcab/elisp/blob/master/valgrind.el

;; https://mostlymaths.net/2010/02/debugging-with-emacsvalgrind.html/

;; valgrind --tool=callgrind --callgrind-out-file=a.log ...

;; https://www.emacswiki.org/emacs/MELPA https://melpa.org/#/getting-started
(require 'package)
(add-to-list 'package-archives '("melpa" . "https://melpa.org/packages/"))
					;(package-initialize)
					;(package-list-packages)
					;(package-refresh-contents)
;(package-install "markdown-mode+")
;(package-install "move-text")

;(setq load-path (cons "~/cxproc-build/site-lisp/ecb" load-path))

					;(require 'cedet)
;(setq ecb-examples-bufferinfo-buffer-name nil)
;(require 'ecb)
(require 'compile)
(require 'gud)
;(require 'cflow-mode)
;(require 'doxymacs)

;(load "xslide/xslide2.el")

;(load "log4e/log4e.el")
;(load "cmake-mode")
;(load "yaxception/yaxception.el")
;(load "deferred/deferred.el")
;(load "deferred/concurrent.el")
;(load "genrnc/genrnc.el")
;(setq genrnc--trang-jar-name "~/cxproc-build/site-lisp/genrnc/trang.jar")
;(setq genrnc--xsd2rng-jar-name "~/cxproc-build/site-lisp/genrnc/xsd2rng.jar")
;(setq genrnc--xsd2rng-xsl-name "~/cxproc-build/site-lisp/genrnc/xsd2rng.xsl")
;(setq genrnc-user-schemas-directory "~/.emacs.d/schema")
;(genrnc-regist-file "~/cxproc-build/trunk/xml/schema/cxp.xsd")

(require 'nxml-mode)
(setq rng-schema-locating-files (append (list (concat (file-name-directory (buffer-file-name)) "../xml/schema/schemas.xml")) rng-schema-locating-files))
(setq nxml-sexp-element-flag t)

(require 'move-text)
(move-text-default-bindings)

; https://www.emacswiki.org/emacs/NxmlMode#toc11
(defun nxml-where ()
      "Display the hierarchy of XML elements the point is on as a path."
      (interactive)
      (let ((path nil))
        (save-excursion
          (save-restriction
            (widen)
            (while (and (< (point-min) (point)) ;; Doesn't error if point is at beginning of buffer
                        (condition-case nil
                            (progn
                              (nxml-backward-up-element) ; always returns nil
                              t)
                          (error nil)))
              (setq path (cons (xmltok-start-tag-local-name) path)))
            (if (called-interactively-p t)
                (message "/%s" (mapconcat 'identity path "/"))
              (format "/%s" (mapconcat 'identity path "/")))))))

;(vc-dir (concat (file-name-directory (buffer-file-name)) ".."))

;; (autoload 'cflow-mode "cflow-mode")

;(load (concat (file-name-directory (buffer-file-name)) "../contrib/pie/elisp/pie.el"))

; (setenv "LD_RUN_PATH" (concat (getenv "HOME") "/target/lib"))
; (setenv "LD_RUN_PATH" "/usr/lib/debug/usr/lib:/usr/lib:/lib")
; (setenv "LD_RUN_PATH" nil)
; (getenv "LD_RUN_PATH")
; (setenv "CXP_DATE" "200109")

(setq prefix
      (concat (file-name-directory (buffer-file-name)) "..")
      )
; (pwd) (cd prefix)

; (getenv "CXP_PATH")
; (setenv "CXP_PATH" (concat prefix "/xml/xsl"))
; (setenv "CXP_PATH" prefix)
; (setenv "CXP_LOG" "3")

;; (setq 
;;  gdb-many-window t
;;  gdb-show-main t
;;  gud-gdb-command-name "gdb -i=mi --annotate=3 -x debug.gdb")

;;; mode

(defun developer-c-mode ()
  ""
  (interactive)
  ;;
  ;(alex-fundamental-mode)
  (c-mode)
  ;;
;  (setq fill-column 70)
  (auto-save-mode 1)
  (setq auto-save-interval 200)
  ;;
  (line-number-mode 1)
  (column-number-mode 1)
;  (setq outline-regexp "^[a-z_]+(.*)")
;  (outline-minor-mode)

  (c-set-style "gnu")

  ;; imenu
  ;; sonst entsprechend imenu-example--* anpassen
;  (setq 
;   imenu-case-fold-search nil
;   ;speedbar-tag-hierarchy-method '(simple-group)
;   speedbar-tag-hierarchy-method '(sort)
;   )
  ;;
  (local-set-key    [f8] 'hide-body)
  ;(local-set-key    [f9] 'show-subtree)
  ;;
  (local-set-key    [f4] 'developer-c-defun-insert)
  (local-set-key    [S-f4] 'developer-c-block-insert)
  ;(local-set-key    [C-f4] 'developer-c-for-insert)
  ;;
;; gud
  (local-set-key    [f5] 'gud-break)
  (local-set-key    [f6] 'gud-step)
  (local-set-key    [f7] 'gud-next)
  (font-lock-mode)
  ;;

  )


(defun developer-c-defun-insert ()
  "fügt eine C-Funktionsdefinition mit dem aktuellen Wort ein."
  (interactive)
  (let ((name-function (current-word)))
    (beginning-of-line)
    (kill-line)
    (insert (format 
	     (concat "\n"
		     "/*! \n"
		     "*/\n"
		     "BOOL_T\n"
		     "%s(void)\n"
		     "{\n"
		     "  return TRUE;\n"
		     "}\n"
		     "/* end of %s() */\n"
		     "\n" )
	     name-function
	     name-function
	     name-function)
	    )
    (previous-line 9)
    (end-of-line)
    (indent-for-tab-command)
    (recenter 10)
    )
  )
; (developer-c-defun-insert)


(defun developer-c-block-insert ()
  "fügt einen Klammer-Block in C-Syntax ein."
  (interactive)
  (let ()

    (if (string-match "\\bif\\b" (current-word))
	(progn
	  (beginning-of-line)
	  (kill-line)
	  (insert "if () {\n}\nelse if () {\n}\nelse {\n}\n")
	  (previous-line 6)
	  (beginning-of-line)
	  (goto-char (+ (point) 4))
	  )
      (if (string-match "\\bfor\\b" (current-word))
	  (progn
	    (beginning-of-line)
	    (kill-line)
	    (insert (format "for (%s=0; %s; %s++) {\n}\n\n" 'i 'i 'i))
	    (previous-line 3)
	    (beginning-of-line)
	    (goto-char (+ (point) 4))
	    )
	(progn
	  (setq p (point))
	  (end-of-line)
	  (insert " () {\n}\n")
	  (goto-char p)
	  ;(forward-word)
	  )

	)
      )					;    (indent-for-tab-command)
    (recenter 10)
    )
  )


(defun developer-c-test-insert ()
  "fügt eine C-Funktionsdefinition mit dem aktuellen Wort ein."
  (interactive)
  (let ((name-function (current-word)))
    (if (string-match "^.*[a-zA-Z]+.*$" name-function)
	(progn
	  (kill-new (format 
		     (concat "\n\n"
			     "  if (RUNTEST) {\n"
			     "    i++;\n"
			     "    printf(\"TEST %%i in '%%s:%%i': %s() = \",i,__FILE__,__LINE__);\n"
			     "\n"
			     "    if (%s()) {\n"
			     "      n_ok++;\n"
			     "      printf(\"OK\\n\");\n"
			     "    }\n"
			     "    else {\n"
			     "      printf(\"Error\\n\");\n"
			     "    }\n"
			     "  }\n"
			     "\n" )
		     name-function
		     name-function)
		    )
	  ;;(forward-word) (yank)
	  )
      )
    )
  )
; (developer-c-test-insert)
; (global-set-key     [S-f9]  'developer-c-test-insert)

;;;
;;; autoload
;;; --------
;;;
(setq auto-mode-alist
        (append '(
		  ("\\.c$"   . developer-c-mode)
		  ("\\.css$"   . developer-c-mode)
		  ("\\.h$"   . developer-c-mode)
		  ) auto-mode-alist
		))

;;; keybindings

(global-set-key [S-f5]  (lambda ()
			  ""
			  (interactive)
			  (beginning-of-buffer)
;; 			  (setq
;; 			   case-fold-search nil
;; 			   tag-list (list  
;;                               "YEAR" "MONTH" "DAY" "WEEK" "CALENDAR" "COL" "AD" "CW" "OW" "OWN" "DOM" "HSTR" "TODAY" "FREE" "NR"
;; 				     )
;; 			   tag-list-i tag-list
;; 			   )
;; 			  (while tag-list-i
;; 			    (beginning-of-buffer)
;; 			    (query-replace (car tag-list-i) (downcase (car tag-list-i)))
;; 			    (setq tag-list-i (cdr tag-list-i))
;; 			    )

			  (query-replace "PIE - Personal Information Environment" 
					 "cxproc - Configurable Xml PROCessor

  Copyright (C) 2006,2007,2008 by Alexander Tenbusch")
			  ;(query-replace "printf_debug(" "printf_debug(1,")
			  (save-buffer)
			  ;(kill-buffer nil)
			  )
		)


(global-set-key [f3]  (lambda ()
			  ""
			  (interactive)
			  (if (buffer-file-name)
			      (save-buffer))
			  (compile (concat "cmake --build " prefix "/../x86_64-gnu-linux/build/ -j 4"
					   ;;" --target cxproc"
					   ;;" --target all" cxproc filex cxproc-cgi cxproc-test 
			  		   ))
					;(compile "make test")
			  )
		)

(global-set-key [C-f3]  (lambda ()
			  ""
			  (interactive)
			  (compile (concat "cmake --build " prefix "/../x86_64-gnu-linux/build/"
					   " --target clean"
			  		   ))
			  )
g		)

(global-set-key [S-f3]  (lambda ()
			""
			(interactive)

			(save-buffer)
			(shell-command 
			 (concat "splint -preproc -globstate -maintype -paramuse -shiftimplementation -exportlocal -formattype -compdef -nullpass -nullassign -mustfreefresh -nullret -retvalint -retvalother -predboolint +boolint -branchstate -warnposixheaders +matchanyintegral -I../srclib/duktape/src -I/usr/include/i386-linux-gnu -I/usr/include/ImageMagick -I/usr/include/libxml2 "
				 ; "*.c *.h"
				 (buffer-file-name)
				 )
			 "splint-out" "splint-err")
			(switch-to-buffer "splint-out")
			(delete-other-windows)
			(grep-mode)
			)
)

(global-set-key [C-f9]  (lambda ()
			""
			(interactive)
			(vc-git-grep
			 (concat "\\b" (current-word) "\\b")
			 "\\*.\\*"
			 prefix)
			)
		)

(global-set-key [S-f9]  (lambda ()
			""
			(interactive)
			
			(setq name
			      (if (region-active-p)
				  (progn
				    (kill-ring-save (region-beginning) (region-end))
				    (current-kill 0))
				(if (current-word)
				    (current-word)
				  (current-kill 0 t))
				)
			      )
			(set-text-properties 0 (length name) nil name)

			(vc-git-grep
			 name
			 "\\*.\\*"
			 prefix)
					;(other-window 1)
					;(search-forward-regexp pattern-line)
			)
)


(setq
 ecb-layout-name "left10"
 ecb-tip-of-the-day nil)
;(ecb-activate)
;(setq speedbar-tag-hierarchy-method
;      '(speedbar-simple-group-tag-hierarchy speedbar-sort-tag-hierarchy))
;(speedbar-frame-mode 1)

;(save-buffer)
;(kill-buffer nil)

; TODO: menu option for switching on/off

(global-set-key      [f1]  'delete-other-windows)
(global-set-key    [M-f1]  'man)

(global-set-key      [f2]  'save-buffer)

(global-set-key [C-prior]  'beginning-of-buffer)
(global-set-key  [C-next]  'end-of-buffer)

(global-set-key     [f10]  'kill-this-buffer)
(global-set-key     [f11]  'alex-shell)

(global-set-key [C-f9] 'developer-find-file-marked)

(global-set-key "\e\?" 'goto-line)

(global-set-key [C-tab] (lambda ()
			 ""
			 (interactive)
			 (switch-to-buffer (other-buffer)))
			 )

(global-set-key [M-right]  'dabbrev-expand)

(global-set-key [C-right]  'forward-word)

;(global-set-key [C-x C-r]  'revert-buffer)

(global-set-key [f5]
		(lambda ()
		  ""
		  (interactive)
		  (shell-command-on-region (point-min) (point-max) "xmllint --format -" nil t)
					;  --dtdvalid h:/tmp/pie.dtd
		   )
		  )

;;; ------------- sonstiges -----------------------------------

(defun developer-find-file-marked ()
  "öffnet eine Datei mit dem markierten Namen."
  (interactive)

  (let ()
    (if (region-active-p)
	(kill-ring-save (region-beginning) (region-end)))
    (setq name (replace-regexp-in-string "^[ \\t]+" "" (current-kill 0)))
    (setq name (replace-regexp-in-string "$[ \\t]+" "" name))
    (set-text-properties 0 (length name) nil name)
    (split-window-below)
    (other-window 1)
    (setq list-name (split-string name ":")) ; not portable to Windows!
    (if (length list-name)
	(if (and 
	     (or (get-buffer (car list-name))
		 (and (setq test-name (concat (file-name-directory (buffer-file-name)) (car list-name)))
		      (file-readable-p test-name)
		      (find-file test-name)))
	     (switch-to-buffer (car list-name)))
	    (if (> (length list-name) 1)
		(goto-line (string-to-number (car (cdr list-name)))))
	  (message "Buffer %s doesnt exist!" (car list-name))
	  )
      )
    )
  )


(defun developer-kill-buffers ()
  "schließt alle Buffer bis auf rest-buffer und den aktuellen Buffer."
  (interactive)
  (let ((rest-buffer '("*scratch*" "*Messages*")))
    
    (dolist (b (buffer-list))
      (if (and (not (member (buffer-name b) rest-buffer))
	       (not (eq b (current-buffer))))
	  (kill-buffer b)
	)
      )
;     (switch-to-buffer (car rest-buffer))
;     (delete-other-windows)
    )
  )
;; (developer-kill-buffers)

;;; Menü Files

(define-key-after
  (lookup-key global-map [menu-bar file])
  [separator-developer] '("--")
  t
  )

(define-key-after
  (lookup-key global-map [menu-bar file])
  [developer-tag] '("Add file tag" . backup-iteration-tag)
  t
  )

(define-key-after
  (lookup-key global-map [menu-bar file])
  [developer-iteraions] '("Show iteration history" . backup-history)
  t
  )

(define-key-after
  (lookup-key global-map [menu-bar file])
  [developer-find-file-marked] '("Open Marked Buffer" . developer-find-file-marked)
  t
  )

(define-key-after
  (lookup-key global-map [menu-bar file])
  [developer-kill] '("Kill Rest Buffers" . developer-kill-buffers)
  t
  )


