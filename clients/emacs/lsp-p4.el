;;; lsp-p4.el --- P4 support for lsp-mode -*- lexical-binding: t -*-

;; Copyright (C) 2018 Dmitri Makarov

;; Author: Dmitri Makarov
;; Version: 1.0
;; Package-Version: 20180408
;; Package-Requires: ((lsp-mode "3.0"))
;; Keywords: p4
;; URL: https://github.com/dmakarov/p4ls

;;; Code:
(require 'lsp-mode)
(require 'lsp-common)

(lsp-define-stdio-client lsp-p4 "p4" (lsp-make-traverser #'(lambda (dir) (directory-files dir nil ".dir-locals.el"))) '("p4lsd"))

(provide 'lsp-p4)
;;; lsp-p4.el ends here
