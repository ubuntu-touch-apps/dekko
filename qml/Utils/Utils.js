var contactIconColors = ["#B68B01", "#CA4B14", "#DB3131", "#D33781", "#6B72C3", "#298BD6", "#2BA098", "#859A01"]

function isMailAddressValid(address) {
    return ! (address === null || address === undefined || address.length === 0)
}

function formatMailAddress(items, nameOnly, addressOnly) {
    if (!isMailAddressValid(items)) {
        return undefined;
    } else if (items[0] !== null && items[0].length > 0) {
        var name = items[0];
        var address = "<a href=\"" + items[2] + "@" + items[3] + "\">"+ " " + items[2] + "@" + items[3] + "</a>";
        if (nameOnly) {
            return name;
        } else if (addressOnly) {
            return address;
        } else {
            return name + address;
        }
    } else {
        return items[2] + "@" + items[3]
    }
}

function formatMailAddresses(addresses, nameOnly, addressOnly) {
    if (!addresses) { return ""; }
    var res = Array()
    for (var i = 0; i < addresses.length; ++i) {
        res.push(formatMailAddress(addresses[i], nameOnly, addressOnly))
    }
    return (addresses.length == 1) ? res[0] : res.join(", ")
}

function formatPlainArrayMailAddresses(addresses, nameOnly, addressOnly) {
    if (!addresses) { return ""; }
    var res = Array()
    for (var i = 0; i < addresses.length; ++i) {
        res.push(formatPlainArrayMailAddress(addresses[i], nameOnly, addressOnly))
    }
    return res;
}

function formatPlainArrayMailAddress(items, nameOnly, addressOnly) {
    if (!isMailAddressValid(items)) {
        return undefined;
    } else if (items[0] !== null && items[0].length > 0) {
        var name = items[0];
        var address = items[2] + "@" + items[3];
        if (nameOnly) {
            return name;
        } else if (addressOnly) {
            return address;
        } else {
            return name + address;
        }
    } else {
        return items[2] + "@" + items[3]
    }
}

function prettyAddress(address) {
    if (!isMailAddressValid(address)) {
        return undefined;
    } else if (address[0] !== null && address[0].length > 0) {
        var name = address[0];
        var address = address[2] + "@" + address[3];
        return name + " <" + address + ">";
    } else {
        return address[2] + "@" + address[3]
    }
}

function prettyDate(date) {
    var today = new Date();
    today.setHours(0);
    today.setMinutes(0);
    today.setSeconds(0);
    today.setMilliseconds(0);

    var lastWeek = new Date(today).setDate(today.getDate() - 6);

    var thisYear = new Date(today);
    thisYear.setMonth(0);
    thisYear.setDate(0);

    if (date >= today)
        return Qt.formatTime(date, "hh:mm");
    else if (date >= lastWeek)
        return Qt.formatDate(date, "ddd");
    else if (date >= thisYear) {
        // It would be better (for readability) to return an ordinal date. 
        // However, I found no library which can give them out localized.
        return Qt.formatDate(date, "dd MMM");
    } else
        return Qt.formatDate(date);
}


function formatDateDetailed(date) {
    // if there's a better way to compare
//    QDateTime::date with "today", well, please do tell me
    return Qt.formatDate(date, "yyyy-MM-dd") == Qt.formatDate(new Date(), "yyyy-MM-dd") ?
                Qt.formatTime(date) :
                Qt.formatDateTime(date)
}

function getIconColor(name, colors) {
    var tmp = 0;
    for (var i = 0; i < name.length; i++) {
        tmp += name.charCodeAt(i);
    }
    return colors[tmp % colors.length];
}

function sectionTypeToSectionName(type) {
    if (type == SectionType.HTML) {
        return qsTr("HTML");
    } else if (type == SectionType.PLAIN_TEXT) {
        return qsTr("Plain text");
    } else if (type == SectionType.ATTACHMENT) {
        return qsTr("Attachments");
    } else {
        console.log("Unknown section type")
    }
}

function abbreviateNameForPrettyAddress(addressInString) {
    if (addressInString.indexOf(" <") > 0) {
        return addressInString.substr(0, addressInString.indexOf(" <"));
    } else {
        return addressInString;
    }
}

function emailAddressFromPrettyAddress(addressInString) {
    if (addressInString.indexOf(" <") > 0) {
        return addressInString.substr(addressInString.indexOf(" <") + 2, addressInString.length - addressInString.indexOf(" <") - 3);
    } else {
        return addressInString;
    }
}
