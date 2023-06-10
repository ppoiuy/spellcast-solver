#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QKeyEvent>
#include <QSlider>
#include <QLabel>
#include <QListWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QFont>
#include <QTableWidget>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QBrush>
#include <QCheckBox>
#include <QRegularExpression>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QMessageBox>
#include <vector>
#include <unordered_map>
#include <utility>
#include "solver.h"
#include "trie.h"


class OutlineDelegate : public QStyledItemDelegate
{
public:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        QStyledItemDelegate::paint(painter, option, index);

        if (option.state & QStyle::State_Selected)
        {
            painter->setPen(Qt::NoPen);
            painter->setBrush(Qt::NoBrush);
            painter->drawRect(option.rect);
        }
        else if (index.data(Qt::UserRole).toBool())
        {
            painter->setPen(QPen(Qt::red, 3));
            painter->setBrush(Qt::NoBrush);
            QRect rect = option.rect.adjusted(1, 1, -1, -1);
            painter->drawRect(rect);
        }
    }
};

class WordDetailsWindow : public QDialog
{
public:
    WordDetailsWindow(const Word& word, QWidget* parent = nullptr) : QDialog(parent), showArrows(true), word(word)
    {
        setFixedSize(224, 269); // Set a static window size

        // Window title is word - value (netGems)
        QString netGemsText = QString::number(word.netGems);
        if (word.netGems > 0)
        {
            netGemsText = QString::fromStdString("+") + netGemsText;
        }
        setWindowTitle(QString::fromStdString(word.word) + " - " + QString::number(word.value) + " (" + netGemsText + ")");

        QVBoxLayout* layout = new QVBoxLayout(this);

        // Create and set up the grid table
        //QTableWidget*
        gridTable = new QTableWidget(this);
        int gridSize = word.grid.size();
        gridTable->setRowCount(gridSize);
        gridTable->setColumnCount(gridSize);
        gridTable->verticalHeader()->setVisible(false);
        gridTable->horizontalHeader()->setVisible(false);

        gridTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // Make the table read-only
        gridTable->setSelectionMode(QAbstractItemView::NoSelection); // Disable tile selection
        gridTable->setFocusPolicy(Qt::NoFocus); // Remove the focus outline

        gridTable->setItemDelegate(new OutlineDelegate); // Set the custom delegate

        for (int i = 0; i < gridSize; ++i)
        {
            gridTable->setColumnWidth(i, 40); // Set the width of each column

            for (int j = 0; j < gridSize; ++j)
            {
                std::string character = std::string(1, word.grid[i][j]);
                character = std::toupper(character[0]); // Convert to uppercase

                QTableWidgetItem* item = new QTableWidgetItem(QString::fromStdString(character));
                item->setTextAlignment(Qt::AlignCenter);

                bool isPath = false;
                bool isSwapped = false;

                // Check if the current coordinates are in the path vector
                for (const auto& pathCoords : word.path)
                {
                    if (pathCoords.first == i && pathCoords.second == j)
                    {
                        isPath = true;
                        break;
                    }
                }

                // Check if the current coordinates are in the swappedTiles vector
                if (word.swappedTiles.count({i,j}) > 0) {
                    isSwapped = true;
                }

                // Set the cell background color based on the condition
                item->setData(Qt::UserRole, isSwapped); // Set custom data to indicate swapped tiles
                if (isPath)
                {
                    item->setBackground(Qt::green);
                    item->setFont(QFont("Arial", 10, QFont::Bold)); // Set path tiles font to bold
                }
                // Add visual indicator for the first tile in the path
                if (isPath && word.path.size() > 0 && word.path[0].first == i && word.path[0].second == j)
                {
                    item->setBackground(QColor(0, 160, 0));
                    item->setForeground(QBrush(Qt::white)); // Set the first tile text color to white
                    item->setFont(QFont("Arial", 10, QFont::Bold)); // Set the first tile font to bold
                }
                // Add arrow indicator for the path direction
                if (showArrows && isPath && word.path.size() > 1)
                {
                    const auto& currentCoords = std::make_pair(i, j);
                    int currentIndex = -1;
                    for (size_t k = 0; k < word.path.size(); ++k)
                    {
                        if (word.path[k] == currentCoords)
                        {
                            currentIndex = static_cast<int>(k);
                            break;
                        }
                    }

                    if (currentIndex != -1)
                    {
                        const auto& nextCoords = word.path[currentIndex + 1];
                        int dx = nextCoords.first - currentCoords.first;
                        int dy = nextCoords.second - currentCoords.second;
                        QString arrowCharacter = getArrowCharacter(dx, dy);
                        item->setText(item->text() + arrowCharacter);
                    }
                }

                gridTable->setItem(i, j, item);
            }
        }

        layout->addWidget(gridTable);

        // Create and set up the text label for word and value
        QLabel* wordLabel = new QLabel(this);
        wordLabel->setText("Word: " + QString::fromStdString(word.word));
        wordLabel->setFont(QFont("Arial", 12, QFont::Bold));
        layout->addWidget(wordLabel);

        QLabel* valueLabel = new QLabel(this);
        valueLabel->setText("Value: " + QString::number(word.value));
        valueLabel->setFont(QFont("Arial", 12));
        layout->addWidget(valueLabel);

        QLabel* netGemLabel = new QLabel(this);
        netGemLabel->setText("Net Gems: " + netGemsText);
        netGemLabel->setFont(QFont("Arial", 12));
        layout->addWidget(netGemLabel);

        // Create the arrow display toggle checkbox
        QCheckBox* arrowCheckBox = new QCheckBox("Show Grid Arrows", this);
        arrowCheckBox->setChecked(showArrows); // Set the default state to checked
        connect(arrowCheckBox, &QCheckBox::toggled, this, &WordDetailsWindow::toggleArrows);
        layout->addWidget(arrowCheckBox); // add checkbox

        setLayout(layout);
    }
private:
    QTableWidget* gridTable;
    bool showArrows; // Added member variable to store the checkbox state
    Word word;
    QString getArrowCharacter(int dx, int dy)
    {
        QString arrowText;
        if (dx == -1 && dy == 0)
            arrowText = "↑";
        else if (dx == 1 && dy == 0)
            arrowText = "↓";
        else if (dx == 0 && dy == -1)
            arrowText = "←";
        else if (dx == 0 && dy == 1)
            arrowText = "→";
        else if (dx == -1 && dy == -1)
            arrowText = "↖";
        else if (dx == -1 && dy == 1)
            arrowText = "↗";
        else if (dx == 1 && dy == -1)
            arrowText = "↙";
        else if (dx == 1 && dy == 1)
            arrowText = "↘";
        return arrowText;
    }
private slots:
    void toggleArrows()
    {
        showArrows = !showArrows;

        // only update current window grid
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(sender());
        if (checkBox)
        {
            // Refresh the table to update arrow visibility
            int gridSize = word.grid.size();
            for (int i = 0; i < gridSize; ++i)
            {
                for (int j = 0; j < gridSize; ++j)
                {
                    QTableWidgetItem* item = gridTable->item(i, j);
                    if (item)
                    {
                        QString text = item->text();
                        bool isPath = false;
                        // Check if the current coordinates are in the path vector
                        for (const auto& pathCoords : word.path)
                        {
                            if (pathCoords.first == i && pathCoords.second == j)
                            {
                                isPath = true;
                                break;
                            }
                        }
                        if (showArrows && isPath && word.path.size() > 1)
                        {
                            const auto& currentCoords = std::make_pair(i, j);
                            int currentIndex = -1;

                            // Find the index of the current coordinates in the word's path
                            for (size_t k = 0; k < word.path.size(); ++k)
                            {
                                if (word.path[k] == currentCoords)
                                {
                                    currentIndex = static_cast<int>(k);;
                                    break;
                                }
                            }

                            if (currentIndex != -1)
                            {
                                const auto& nextCoords = word.path[currentIndex + 1];
                                int dx = nextCoords.first - currentCoords.first;
                                int dy = nextCoords.second - currentCoords.second;

                                QString arrowCharacter = getArrowCharacter(dx, dy);
                                text.append(arrowCharacter);
                            }
                        }
                        else
                        {
                            text.remove(QRegularExpression("[\u2190-\u2199]")); // Remove arrow characters
                        }

                        item->setText(text);
                    }
                }
            }

        }


    }
};

class SquareTextBox : public QLineEdit
{
public:
    SquareTextBox(QWidget* parent = nullptr) : QLineEdit(parent)
    {
        setMaxLength(1);
        setAlignment(Qt::AlignCenter);
        setStyleSheet("QLineEdit { border: 1px solid black; }");
    }

    QSize sizeHint() const override
    {
        int sideLength = qMin(width(), height());
        return QSize(sideLength, sideLength);
    }

protected:
    void keyPressEvent(QKeyEvent* event) override
    {
        if (event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z)
        {
            setText(event->text().toUpper());
            for (int i = 0; i < 4; i++) {
                focusNextChild();
            }
        }
        else if (event->key() == Qt::Key_Backspace) {
            if (text().isEmpty())
            {
                for (int i = 0; i < 4; i++) {
                    focusPreviousChild();
                }
            }
            else
            {
                QLineEdit::keyPressEvent(event);
            }
        }
        else if (event->key() == Qt::Key_Up){
            for (int i = 0; i < 20; i++) {
                focusPreviousChild();
            }
        }
        else if (event->key() == Qt::Key_Down){
            for (int i = 0; i < 20; i++) {
                focusNextChild();
            }
        }
        else if (event->key() == Qt::Key_Left){
            for (int i = 0; i < 4; i++) {
                focusPreviousChild();
            }
        }
        else if (event->key() == Qt::Key_Right){
            for (int i = 0; i < 4; i++) {
                focusNextChild();
            }
        }
    }
};

class GridWindow : public QWidget
{
public:
    GridWindow(QWidget* parent = nullptr) : QWidget(parent), limitMultipliers(true)
    {
        QGridLayout* layout = new QGridLayout(this);
        layout->setSpacing(1);

        grid.resize(5, std::vector<char>(5));

        for (int row = 0; row < 5; ++row)
        {
            for (int col = 0; col < 5; ++col)
            {
                QWidget* tileWidget = new QWidget(this);
                QGridLayout* tileLayout = new QGridLayout(tileWidget);
                tileLayout->setSpacing(0);

                SquareTextBox* textBox = new SquareTextBox(tileWidget);
                textBox->setObjectName(QString("textBox_%1_%2").arg(row).arg(col));
                tileLayout->addWidget(textBox, 0, 0); // Position text box

                QPushButton* cycleButton = new QPushButton("SL", tileWidget);
                cycleButton->setProperty("row", row);
                cycleButton->setProperty("col", col);
                cycleButton->setProperty("state", 0); // 0 = SL, 1 = DL, 2 = TL
                cycleButton->setFixedSize(20, 20); // Set button size
                connect(cycleButton, &QPushButton::clicked, this, &GridWindow::cycleState);
                tileLayout->addWidget(cycleButton, 0, 1, Qt::AlignTop | Qt::AlignRight); // Position button to top right

                QPushButton* multiplierButton = new QPushButton("1x", tileWidget);
                multiplierButton->setProperty("row", row);
                multiplierButton->setProperty("col", col);
                multiplierButton->setProperty("state", 0); // 0 = 1x, 1 = 2x
                multiplierButton->setFixedSize(20, 20); // Set button size
                connect(multiplierButton, &QPushButton::clicked, this, &GridWindow::toggleMultiplier);
                tileLayout->addWidget(multiplierButton, 0, 2, Qt::AlignTop | Qt::AlignRight); // Position button to top right

                QPushButton* gemButton = new QPushButton("", tileWidget);
                gemButton->setProperty("row", row);
                gemButton->setProperty("col", col);
                gemButton->setProperty("state", 0); // 0 = no gem, 1 = gem
                gemButton->setFixedSize(40, 20); // Set button size
                connect(gemButton, &QPushButton::clicked, this, &GridWindow::toggleGemPosition);
                tileLayout->addWidget(gemButton, 1, 0, Qt::AlignTop);

                tileWidget->setLayout(tileLayout);
                layout->addWidget(tileWidget, row, col);

                cycleButtons[row][col] = cycleButton;
                multiplierButtons[row][col] = multiplierButton;
                gemButtons[row][col] = gemButton;
                grid[row][col] = ' '; // Initialize grid with empty characters

                // Connect the textChanged signal of the SquareTextBox to updateGrid slot
                connect(textBox, &SquareTextBox::textChanged, [this, row, col](const QString& text){
                    updateGrid(row, col, text);
                });
            }
        }

        QSlider* gemSlider = new QSlider(Qt::Horizontal, this);
        gemSlider->setRange(0, 10);
        gemSlider->setFixedWidth(150); // Set the width of the slider
        gemSlider->setValue(3); // Set the initial value of the slider
        connect(gemSlider, &QSlider::valueChanged, this, &GridWindow::updateGemValue);
        layout->addWidget(gemSlider, 6, 0, 1, 5);

        gemValueLabel = new QLabel("Gems: 3 Swaps: 1 (Fast)", this);
        layout->addWidget(gemValueLabel, 7, 0, 1, 5);

        QPushButton* solveButton = new QPushButton("Solve", this);
        connect(solveButton, &QPushButton::clicked, this, &GridWindow::runSolver);
        layout->addWidget(solveButton, 5, 0, 1, 5, Qt::AlignBottom | Qt::AlignLeft); // Position solve button

        // Create a QLabel for the "Minimum Gems Kept" label
        QLabel* minGemsKeptLabel = new QLabel("Min Gems Kept: ");
        layout->addWidget(minGemsKeptLabel, 7, 2, 1, 1, Qt::AlignBottom | Qt::AlignRight); // Position at the bottom left with left alignment

        // Create the arrow display toggle button
        QLineEdit* minGemsKeptLineEdit = new QLineEdit();
        minGemsKeptLineEdit->setValidator(new QIntValidator()); // Only accept integer values
        minGemsKeptLineEdit->setText("0"); // Set default text

        // Set the maximum width and height for the text box
        minGemsKeptLineEdit->setMaximumWidth(30);
        minGemsKeptLineEdit->setMaximumHeight(20);

        QObject::connect(minGemsKeptLineEdit, &QLineEdit::textChanged, this, [=](const QString& text) {
            bool conversionOk;
            int newMinGemsKept = text.toInt(&conversionOk);
            if (conversionOk) {
                minGemsKept = newMinGemsKept;
            }
        });
        layout->addWidget(minGemsKeptLineEdit, 7, 3, 1, 1, Qt::AlignBottom); // Position text box

        // Create a QLabel for the "Number of Words" label
        QLabel* numScoresLabel = new QLabel("# of words: ");
        layout->addWidget(numScoresLabel, 7, 3, 1, 1, Qt::AlignBottom | Qt::AlignRight); // Position at the bottom left with left alignment

        // Create the arrow display toggle button
        QLineEdit* scoreNumberLineEdit = new QLineEdit();
        scoreNumberLineEdit->setValidator(new QIntValidator()); // Only accept integer values
        scoreNumberLineEdit->setText("50"); // Set default text

        // Set the maximum width and height for the text box
        scoreNumberLineEdit->setMaximumWidth(65);
        scoreNumberLineEdit->setMaximumHeight(20);

        QObject::connect(scoreNumberLineEdit, &QLineEdit::textChanged, this, [=](const QString& text) {
            bool conversionOk;
            int newScoreNumber = text.toInt(&conversionOk);
            if (conversionOk) {
                scoreNumber = newScoreNumber;
            }
        });
        layout->addWidget(scoreNumberLineEdit, 7, 4, 1, 1, Qt::AlignBottom); // Position text box

        QLabel *listTitleLabel = new QLabel("Highest Value Words", this);
        listTitleLabel->setAlignment(Qt::AlignCenter);
        listTitleLabel->setFont(QFont("Arial", 12, QFont::Bold));
        layout->addWidget(listTitleLabel, 0, 5, 1, 1);

        // List widget to display the elements
        listWidget = new QListWidget(this);
        layout->addWidget(listWidget, 1, 5, 10, 1);

        // Connect itemClicked signal to showWordDetails slot
        connect(listWidget, &QListWidget::itemClicked, this, &GridWindow::showWordDetails);

        // Create the arrow display toggle checkmark
        QCheckBox* limitMultipliersCheckBox = new QCheckBox("Limit Multipliers", this);
        limitMultipliersCheckBox->setChecked(limitMultipliers); // Set the default state to checked
        connect(limitMultipliersCheckBox, &QCheckBox::toggled, this, &GridWindow::toggleLimitMultipliers);
        layout->addWidget(limitMultipliersCheckBox, 5, 3, 1, 5); // add checkbox

        // reset multipliers
        QPushButton *resetMultButton = new QPushButton("Reset Multipliers",this);
        connect(resetMultButton, &QPushButton::clicked, this, &GridWindow::resetCycleButtons);
        connect(resetMultButton, &QPushButton::clicked, this, &GridWindow::resetMultiplierButtons);
        //resetMultButton->setFixedWidth(100);
        layout->addWidget(resetMultButton, 6, 3, 1, 1);

        // reset multipliers
        QPushButton *resetGemButton = new QPushButton("Reset Gem States",this);
        connect(resetGemButton, &QPushButton::clicked, this, &GridWindow::resetGemButtons);
        layout->addWidget(resetGemButton, 6, 2, 1, 1);

        // reset grid and multipliers
        QPushButton *resetGridButton = new QPushButton("Reset Grid",this);
        connect(resetGridButton, &QPushButton::clicked, this, &GridWindow::resetCycleButtons);
        connect(resetGridButton, &QPushButton::clicked, this, &GridWindow::resetMultiplierButtons);
        connect(resetGridButton, &QPushButton::clicked, this, &GridWindow::resetGemButtons);
        connect(resetGridButton, &QPushButton::clicked, this, &GridWindow::resetGrid);
        resetGridButton->setFixedWidth(65);
        layout->addWidget(resetGridButton, 6, 4, 1, 1);

        setLayout(layout);

        initializeValidWordTrie(validWords);
        initializeValues(letterValues, letterMultipliers, wordMultipliers, gemPositions);
    }

private slots:
    void updateGrid(int row, int col, const QString& text)
    {
        // Update the grid matrix with the text from the text box
        grid[row][col] = text.isEmpty() ? ' ' : text.at(0).toUpper().toLatin1();
    }

    void updateGemValue(int value)
    {
        gemsValue = value;
        gemValueLabel->setText(QString("Gems: %1").arg(value));

        gemValueLabel->setText(gemValueLabel->text()+QString(" Swaps: %1").arg(value/3));

        if ((int)(value/3) >= 3)
        {
            gemValueLabel->setText(gemValueLabel->text() + " (Very Slow)");
        }
        else if ((int)(value/3) >= 2)
        {
            gemValueLabel->setText(gemValueLabel->text() + " (Slow)");
        }
        else if ((int)(value/3) >= 1)
        {
            gemValueLabel->setText(gemValueLabel->text() + " (Fast)");
        }
        else if ((int)(value/3) >= 0)
        {
            gemValueLabel->setText(gemValueLabel->text() + " (Very Fast)");
        }

        if (value >= 9)
        {
            gemValueLabel->setStyleSheet("color: red");
        }
        else
        {
            gemValueLabel->setStyleSheet("");
        }
    }

    void toggleLimitMultipliers()
    {
        limitMultipliers = !limitMultipliers;
        // reset buttons
        if (limitMultipliers)
        {
            resetCycleButtons();
            resetMultiplierButtons();
        }
    }

    void runSolver()
    {
        // Check if letterMultipliers matrix is only filled with 1s
        bool noLetterMultipliers = true;
        for (int row = 0; row < 5; ++row)
        {
            for (int col = 0; col < 5; ++col)
            {
                if (letterMultipliers[row][col] != 1)
                {
                    noLetterMultipliers = false;
                    break;
                }
            }
            if (!noLetterMultipliers)
                break;
        }

        // Show a warning message if all letterMultipliers are 1s
        if (noLetterMultipliers)
        {
            QMessageBox::warning(this, "Warning", "No letter multipliers set.");
        }

        std::vector<std::vector<char>> gridMatrix;
        gridMatrix.resize(5, std::vector<char>(5));

        for (int row = 0; row < 5; ++row)
        {
            for (int col = 0; col < 5; ++col)
            {
                gridMatrix[row][col] = tolower(grid[row][col]);
            }
        }

        // Clear the list widget
        listWidget->clear();

        bool checkLowerSwaps = true;
        int maxSwaps = int(gemsValue/3);
        int scoreNumberCount = 0;

        while (checkLowerSwaps)
        {
            topWordsVect.clear();
            solve(topWordsVect, gridMatrix, maxSwaps, validWords, letterValues, letterMultipliers, wordMultipliers, gemPositions, scoreNumber, gemsValue, minGemsKept);

            // Populate the list widget with the elements
            for (const Word& word : topWordsVect) {
                //if ((word.netGems + gemsValue) >= minGemsKept) { // only list words that keep gems at or above minGemsKept
                QListWidgetItem* item = new QListWidgetItem();
                QString netGemsText = QString::number(word.netGems);
                if (word.netGems > 0)
                {
                    netGemsText = QString::fromStdString("+") + netGemsText;
                }
                QString text = QString::fromStdString(word.word) + " - " + QString::number(word.value) + " (" + netGemsText + ")";
                item->setText(text);
                item->setData(Qt::UserRole, QVariant::fromValue(word));
                listWidget->addItem(item);
                scoreNumberCount++;
                //}
            }

            // run loop again if there are lower numbers of swaps to check
            if (maxSwaps > 0 && scoreNumberCount < scoreNumber)
            {
                maxSwaps--;
            }
            else
            {
                checkLowerSwaps = false;
            }

        }
        std::cout << "DONE\n";

    }

    void cycleState()
    {
        QPushButton* button = qobject_cast<QPushButton*>(sender());
        if (button)
        {
            int row = button->property("row").toInt();
            int col = button->property("col").toInt();
            int state = button->property("state").toInt();

            if (limitMultipliers)
            {
                resetCycleButtons();
            }

            state = (state + 1) % 3; // Cycle between 0, 1, and 2 (SL, DL, TL)

            button->setProperty("state", state);
            switch (state)
            {
            case 0:
                button->setText("SL");
                button->setStyleSheet(""); // Reset button color
                // Assign SL in letterMultipliers matrix
                letterMultipliers[row][col] = 1;
                break;
            case 1:
                button->setText("DL");
                button->setStyleSheet("background-color: yellow"); // Set button color to yellow
                // Assign DL in letterMultipliers matrix
                letterMultipliers[row][col] = 2;
                break;
            case 2:
                button->setText("TL");
                button->setStyleSheet("background-color: red"); // Set button color to red
                // Assign TL in letterMultipliers matrix
                letterMultipliers[row][col] = 3;
                break;
            }
        }
    }

    void toggleMultiplier()
    {
        QPushButton* button = qobject_cast<QPushButton*>(sender());
        if (button)
        {
            int row = button->property("row").toInt();
            int col = button->property("col").toInt();
            int state = button->property("state").toInt();

            if (limitMultipliers)
            {
                resetMultiplierButtons();
            }

            state = (state + 1) % 2; // Cycle between 0 and 1 (1x, 2x)

            button->setProperty("state", state);
            switch (state)
            {
            case 0:
                button->setText("1x");
                button->setStyleSheet(""); // Reset button color
                // Assign false in wordMultipliers matrix
                wordMultipliers[row][col] = false;
                break;
            case 1:
                button->setText("2x");
                button->setStyleSheet("background-color: magenta"); // Set button color to magenta
                // Assign true in wordMultipliers matrix
                wordMultipliers[row][col] = true;
                break;
            }
        }
    }

    void toggleGemPosition()
    {
        QPushButton* button = qobject_cast<QPushButton*>(sender());
        if (button)
        {
            int row = button->property("row").toInt();
            int col = button->property("col").toInt();
            int state = button->property("state").toInt();

            state = (state + 1) % 2; // Cycle between 0 and 1 (1x, 2x)

            button->setProperty("state", state);
            switch (state)
            {
            case 0:
                button->setText("");
                button->setStyleSheet(""); // Reset button color
                // Assign false in gemPositions matrix
                gemPositions[row][col] = false;
                break;
            case 1:
                button->setText("");
                button->setStyleSheet("background-color: pink"); // Set button color to pink
                // Assign true in gemPositions matrix
                gemPositions[row][col] = true;
                break;
            }
        }
    }

    void showWordDetails(QListWidgetItem* item)
    {
        Word word = item->data(Qt::UserRole).value<Word>();

        // Create and show a new window to display the word details
        WordDetailsWindow* detailsWindow = new WordDetailsWindow(word, this);
        detailsWindow->show();
    }

private:
    std::vector<std::vector<char>> grid;

    Trie validWords;
    std::map<char, int> letterValues;
    std::vector<std::vector<int>> letterMultipliers;
    std::vector<std::vector<bool>> wordMultipliers;
    std::vector<std::vector<bool>> gemPositions;
    std::vector<Word> topWordsVect;
    int minGemsKept = 0;
    int scoreNumber = 50;
    int gemsValue = 3;
    QLabel* gemValueLabel;

    QPushButton* cycleButtons[5][5];
    QPushButton* multiplierButtons[5][5];
    QPushButton* gemButtons[5][5];

    bool showArrows;
    bool limitMultipliers;

    QListWidget* listWidget;

    void resetCycleButtons()
    {
        for (int row = 0; row < 5; ++row)
        {
            for (int col = 0; col < 5; ++col)
            {
                QPushButton* button = cycleButtons[row][col];
                button->setProperty("state", 0);
                button->setText("SL");
                button->setStyleSheet(""); // Reset button color
                letterMultipliers[row][col] = 1;
            }
        }
    }

    void resetMultiplierButtons()
    {
        for (int row = 0; row < 5; ++row)
        {
            for (int col = 0; col < 5; ++col)
            {
                QPushButton* button = multiplierButtons[row][col];
                button->setProperty("state", 0);
                button->setText("1x");
                button->setStyleSheet(""); // Reset button color
                wordMultipliers[row][col] = false;
            }
        }
    }

    void resetGemButtons()
    {
        for (int row = 0; row < 5; ++row)
        {
            for (int col = 0; col < 5; ++col)
            {
                QPushButton* button = gemButtons[row][col];
                button->setProperty("state", 0);
                button->setText("");
                button->setStyleSheet(""); // Reset button color
                gemPositions[row][col] = false;
            }
        }
    }

    void resetGrid()
    {
        for (int row = 0; row < 5; ++row)
        {
            for (int col = 0; col < 5; ++col)
            {
                QString objectName = QString("textBox_%1_%2").arg(row).arg(col);
                SquareTextBox* textBox = findChild<SquareTextBox*>(objectName);
                if (textBox)
                {
                    textBox->clear();
                }
            }
        }
    }
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    GridWindow window;
    window.setWindowTitle("SpellCast Solver v1.4");
    // v1.4

    // Set the fixed window size
    const int WINDOW_WIDTH = 680;
    const int WINDOW_HEIGHT = 440;
    window.setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    window.show();

    return app.exec();
}
