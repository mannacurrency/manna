#! /usr/bin/python3

"""Create and parse 'send'-type messages."""

import struct

from . import (util, config, exceptions, bitcoin, util)

ID = 0


def validate (db, source, asset, quantity):
    problems = []
    if asset == 'BTC': problems.append('cannot send bitcoins')  # Only for parsing.
    if quantity < 0: problems.append('negative quantity')
    return problems

def compose (db, source, destinations, asset, quantity):
    cursor = db.cursor()

    if not isinstance(quantity, int):
        problems.append('quantity must be in satoshis')
        return problems

    # Just send BTC?
    if asset == 'BTC':
        # TODO
        return (source, [(destination, quantity)], None)
    
    #quantity must be in int satoshi (not float, string, etc)
    if not isinstance(quantity, int):
        raise exceptions.SendError('quantity must be an int (in satoshi)')

    # Only for outgoing (incoming will overburn).
    balances = list(cursor.execute('''SELECT * FROM balances WHERE (address = ? AND asset = ?)''', (source, asset)))
    if not balances or balances[0]['quantity'] < quantity:
        raise exceptions.SendError('insufficient funds')

    problems = validate(db, source, asset, quantity)
    if problems: raise exceptions.SendError(problems)

    asset_id = util.asset_id(asset)
    data = config.PREFIX + struct.pack(config.TXTYPE_FORMAT, ID)
    data += struct.pack(FORMAT, asset_id, quantity)

    cursor.close()
    return (source, [(destinations, None)], data)

def parse (db, tx, message):
    cursor = db.cursor()

    # Unpack message.
    number_of_outputs = int(len(message) / 16)
    destinations = tx['destination'].split(' ')
    assert number_of_outputs == len(destinations) 
    try:
        array = struct.unpack('>' + 'QQ' * number_of_outputs, message)
        status = 'valid'
    except (AssertionError, struct.error):
        array = []
        status = 'invalid: could not unpack'
    outputs = []
    for i in range(int(len(array) / 2)):
        asset_id = array[2 * i]
        quantity = array[(2 * i) + 1]
        outputs.append((destinations[i], util.asset_name(asset_id), quantity))

    if status == 'valid':
        # Oversend
        new_outputs = []
        for (destination, asset, quantity) in outputs:
            cursor.execute('''SELECT * FROM balances \
                              WHERE (address = ? AND asset = ?)''', (tx['source'], asset))
            balances = cursor.fetchall()

            if not balances:  quantity = 0
            elif balances[0]['quantity'] < quantity:
                quantity = min(balances[0]['quantity'], quantity)
            # For SQLite3
            quantity = min(quantity, config.MAX_INT)

            problems = validate(db, tx['source'], asset, quantity)
            if problems:
                status = 'invalid: ' + '; '.join(problems)
                break
            new_outputs.append((destination, asset, quantity))
        outputs = new_outputs

    if status == 'valid':
        for (destination, asset, quantity) in outputs:
            util.debit(db, tx['block_index'], tx['source'], asset, quantity, event=tx['tx_hash'])
            util.credit(db, tx['block_index'], destination, asset, quantity, event=tx['tx_hash'])

    # Add parsed transaction to message-type–specific table.
    bindings = {
        'tx_index': tx['tx_index'],
        'tx_hash': tx['tx_hash'],
        'block_index': tx['block_index'],
        'source': tx['source'],
        'destinations': tx['destination'],
        'assets': ' '.join([str(element[1]) for element in outputs]),
        'quantities': ' '.join([str(element[2]) for element in outputs]),
        'status': status,
    }
    sql='insert into sends values(:tx_index, :tx_hash, :block_index, :source, :destinations, :assets, :quantities, :status)'
    cursor.execute(sql, bindings)

    cursor.close()

# vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4
