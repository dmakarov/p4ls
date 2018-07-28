;;; lsp-p4.el --- P4 support for lsp-mode -*- lexical-binding: t -*-

;; Copyright (C) 2018 Dmitri Makarov

;; Author: Dmitri Makarov
;; Version: 1.0
;; Package-Requires: ((lsp-mode "3.0"))
;; Keywords: lsp, p4
;; URL: https://github.com/dmakarov/p4ls

;;; Commentary:

;;; To enable lsp-p4 include the following lisp code in init.el after
;;; loading lsp-mode
;;;
;;;    (with-eval-after-load 'lsp-mode
;;;      (require 'lsp-p4)
;;;      (add-hook 'p4-mode-hook #'lsp-p4-enable)
;;;
;;; See `lsp-p4lsd-executable' to customize the path to p4lsd.

;;; Code:

(require 'lsp-mode)
(require 'lsp-common)

(defgroup lsp-p4 nil
  "Customization variables for lsp-p4."
  :group 'tools)

(defcustom lsp-p4lsd-executable
  "p4lsd"
  "The p4lsd executable."
  :type 'string
  :group 'lsp-p4)

(lsp-define-stdio-client lsp-p4
                         "p4"
                         (lsp-make-traverser "compile_commands.json")
                         (list lsp-p4lsd-executable)
                         :ignore-regexps
                         '("^Error -[0-9]+: .+$"))

(provide 'lsp-p4)

;;; lsp-p4.el ends here
