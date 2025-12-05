#pragma once
/**
 * @file html_item_delegate.h
 * @brief Custom Delegate für HTML-formatierte ListWidget Items
 * 
 * STRICT LEGACY: Ermöglicht mehrfarbige Einträge wie im Original
 */

#include <QStyledItemDelegate>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QApplication>

namespace astro {

/**
 * @brief Delegate für HTML-formatierte ListWidget Items
 */
class HtmlItemDelegate : public QStyledItemDelegate {
public:
    explicit HtmlItemDelegate(QObject* parent = nullptr) 
        : QStyledItemDelegate(parent) {}
    
    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override {
        
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        
        // Hintergrund zeichnen
        if (opt.state & QStyle::State_Selected) {
            painter->fillRect(opt.rect, opt.palette.highlight());
        } else if (opt.backgroundBrush.style() != Qt::NoBrush) {
            painter->fillRect(opt.rect, opt.backgroundBrush);
        }
        
        // HTML-Text rendern
        QTextDocument doc;
        doc.setDefaultFont(opt.font);
        doc.setHtml(opt.text);
        
        painter->save();
        painter->translate(opt.rect.topLeft());
        painter->setClipRect(opt.rect.translated(-opt.rect.topLeft()));
        
        QAbstractTextDocumentLayout::PaintContext ctx;
        if (opt.state & QStyle::State_Selected) {
            ctx.palette.setColor(QPalette::Text, opt.palette.highlightedText().color());
        }
        
        doc.documentLayout()->draw(painter, ctx);
        painter->restore();
    }
    
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        
        QTextDocument doc;
        doc.setDefaultFont(opt.font);
        doc.setHtml(opt.text);
        
        return QSize(doc.idealWidth(), doc.size().height());
    }
};

} // namespace astro
